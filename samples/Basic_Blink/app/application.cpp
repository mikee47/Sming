#include <SmingCore.h>

#include <Storage/SpiFlash.h>
#include <LittleFS.h>
#include <IFS/FileCopier.h>

enum class MigrateResult {
	failure,
	alreadyUpgraded,
	success,
};

String toString(MigrateResult r)
{
	switch(r) {
	case MigrateResult::failure:
		return "failure";
	case MigrateResult::alreadyUpgraded:
		return "Already upgraded";
	case MigrateResult::success:
		return "success";
	}
	return "BAD RESULT";
}

namespace
{
MigrateResult migrateFilesystem()
{
	/*
	 * Locate the original SPIFFS partition we intend to upgrade
	 */
	auto targetPart = Storage::findPartition(F("spiffs0"));
	if(!targetPart) {
		// Not found: presumably already upgraded
		return MigrateResult::alreadyUpgraded;
	}
	if(targetPart.address() != 0x100000) {
		// Bad offset, unsafe to proceed
		return MigrateResult::failure;
	}

	/*
	 * Locate the source SPIFFS partition and mount it
	 */
	auto sourcePart = Storage::findPartition(F("spiffs1"));
	if(!sourcePart) {
		// Cannot proceed
		return MigrateResult::failure;
	}

	std::unique_ptr<IFS::FileSystem> srcFilesystem{IFS::createSpiffsFilesystem(sourcePart)};
	if(!srcFilesystem) {
		return MigrateResult::failure;
	}
	if(srcFilesystem->mount() < 0) {
		return MigrateResult::failure;
	}

	/*
	 * Construct new partition information so we can mount it as LittleFS and extend the size
	 * This does not change the on-flash structures; we update those later when everything's
	 * been successfully copied over.
	 */
	using Partition = Storage::Partition;
	Partition::Info info(F("lfs0"), Partition::SubType::Data::littlefs, targetPart.address(), 0x200000);
	targetPart = Partition(*Storage::spiFlash, info);

	/*
	 * Create the target LittleFS filesystem, format then mount
	 */
	std::unique_ptr<IFS::FileSystem> dstFilesystem{IFS::createLfsFilesystem(targetPart)};
	if(!dstFilesystem) {
		// Cannot proceed
		return MigrateResult::failure;
	}
	if(dstFilesystem->format() < 0) {
		return MigrateResult::failure;
	}
	if(dstFilesystem->mount() < 0) {
		return MigrateResult::failure;
	}

	// Copy files
	IFS::FileCopier copier(*srcFilesystem, *dstFilesystem);

	copier.onError([&](const IFS::FileCopier::ErrorInfo& info) -> bool {
		IFS::FileSystem::Info fsi{};
		info.fileSys.getinfo(fsi);

		/*
		 * SPIFFS doesn't have a root directory entry
		 */
		if(fsi.type == IFS::FileSystem::Type::SPIFFS && info.operation == IFS::FileCopier::Operation::open &&
		   !info.path) {
			return true;
		}

		Serial << fsi.partition.name() << ": " << info << endl;
		return false;
	});

	if(!copier.copyDir(nullptr, nullptr)) {
		// Copy failed
		return MigrateResult::failure;
	}

	//
	// TODO: Consider verifying file contents
	//

	//
	// TODO: Write modified partition table to flash
	//

	// Reboot
	System.restart();
	return MigrateResult::success;
}

} // namespace

void init()
{
	Serial.begin(COM_SPEED_SERIAL);
	Serial.systemDebugOutput(true);

	auto result = migrateFilesystem();
	Serial << "Migrate result: " << result << endl;
}
