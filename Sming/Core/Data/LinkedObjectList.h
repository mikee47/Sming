/****
 * Sming Framework Project - Open Source framework for high efficiency native ESP8266 development.
 * Created 2015 by Skurydin Alexey
 * http://github.com/SmingHub/Sming
 * All files of the Sming Core are provided under the LGPL v3 license.
 *
 * LinkedObjectList.h
 *
 ****/
#pragma once

#include "LinkedObject.h"

/**
 * @brief Singly-linked list of objects
 * @note We don't own the items, just keep references to them
 */
class LinkedObjectList
{
public:
	constexpr LinkedObjectList()
	{
	}

	LinkedObjectList(const LinkedObjectList& other) = delete;

	constexpr LinkedObjectList(LinkedObject* object) : mHead(object)
	{
	}

	bool add(LinkedObject* object);

	bool add(const LinkedObject* object)
	{
		return add(const_cast<LinkedObject*>(object));
	}

	bool insert(LinkedObject* object)
	{
		if(object == nullptr) {
			return false;
		}

		object->mNext = mHead;
		mHead = object;
		return true;
	}

	bool insert(const LinkedObject* object)
	{
		return insert(const_cast<LinkedObject*>(object));
	}

	bool remove(LinkedObject* object);

	LinkedObject* pop()
	{
		if(mHead == nullptr) {
			return nullptr;
		}
		auto obj = mHead;
		mHead = mHead->mNext;
		obj->mNext = nullptr;
		return obj;
	}

	void clear()
	{
		mHead = nullptr;
	}

	LinkedObject* head()
	{
		return mHead;
	}

	const LinkedObject* head() const
	{
		return mHead;
	}

	LinkedObject* tail()
	{
		if(mHead == nullptr) {
			return nullptr;
		}
		auto it = mHead;
		while(it->mNext != nullptr) {
			it = it->mNext;
		}
		return it;
	}

	const LinkedObject* tail() const
	{
		return const_cast<LinkedObjectList*>(this)->tail();
	}

	bool isEmpty() const
	{
		return mHead == nullptr;
	}

	int indexOf(const LinkedObject* obj) const
	{
		unsigned n{0};
		for(auto p = mHead; p != nullptr; p = p->mNext, ++n) {
			if(p == obj) {
				return n;
			}
		}
		return -1;
	}

protected:
	LinkedObject* mHead{nullptr};
};

template <typename ObjectType> class LinkedObjectListTemplate : public LinkedObjectList
{
public:
	constexpr LinkedObjectListTemplate() = default;

	constexpr LinkedObjectListTemplate(ObjectType* object) : LinkedObjectList(object)
	{
	}

	ObjectType* head()
	{
		return reinterpret_cast<ObjectType*>(mHead);
	}

	const ObjectType* head() const
	{
		return reinterpret_cast<const ObjectType*>(mHead);
	}

	ObjectType* tail()
	{
		return reinterpret_cast<ObjectType*>(LinkedObjectList::tail());
	}

	const ObjectType* tail() const
	{
		return reinterpret_cast<const ObjectType*>(LinkedObjectList::tail());
	}

	typename ObjectType::Iterator begin()
	{
		return head();
	}

	typename ObjectType::Iterator end()
	{
		return nullptr;
	}

	typename ObjectType::ConstIterator begin() const
	{
		return head();
	}

	typename ObjectType::ConstIterator end() const
	{
		return nullptr;
	}

	template <typename... ParamTypes> bool add(ParamTypes... params)
	{
		return LinkedObjectList::add(params...);
	}

	template <typename... ParamTypes> bool insert(ParamTypes... params)
	{
		return LinkedObjectList::insert(params...);
	}

	ObjectType* pop()
	{
		return reinterpret_cast<ObjectType*>(LinkedObjectList::pop());
	}

	size_t count() const
	{
		return std::count_if(begin(), end(), [](const ObjectType&) { return true; });
	}

	bool contains(const ObjectType& object) const
	{
		return std::find(begin(), end(), object);
	}
};

/**
 * @brief Class template for singly-linked list of objects
 * @note We own the objects so are responsible for destroying them when removed
 */
template <typename ObjectType> class OwnedLinkedObjectListTemplate : public LinkedObjectListTemplate<ObjectType>
{
public:
	OwnedLinkedObjectListTemplate()=default;

	OwnedLinkedObjectListTemplate(OwnedLinkedObjectListTemplate&& other)
	{
		this->mHead = other.mHead;
		other.mHead = nullptr;
	}

	~OwnedLinkedObjectListTemplate()
	{
		clear();
	}

	bool remove(ObjectType* object)
	{
		bool res = LinkedObjectList::remove(object);
		delete object;
		return res;
	}

	void clear()
	{
		while(remove(this->head())) {
			//
		}
	}
};
