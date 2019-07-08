#
# Custom roles for simplified linking to parts of Sming
#
# autolink from https://github.com/espressif/esp-idf/blob/master/docs/link-roles.py
#
# doclink mikee47 <mike@sillyhouse.net>
#

import re
import os
import sys
from docutils import nodes, utils
from sphinx import roles, addnodes
from sphinx.util.nodes import set_role_source_info, split_explicit_title

baseurl = 'https://github.com/SmingHub/Sming'

def run_cmd_get_output(cmd):
    return os.popen(cmd).read().strip()

def get_github_rev():
    path = run_cmd_get_output('git rev-parse --short HEAD')
    tag = run_cmd_get_output('git describe --exact-match')
    print('Git commit ID: ', path)
    if len(tag):
        print('Git tag: ', tag)
        path = tag
    return path


def setup(app):
    sming_home = os.environ.get('SMING_HOME', None)
    if sming_home:
        smingdir = os.path.abspath(sming_home + '/..')
        print('SMINGDIR = ' + smingdir)
        basepath = 'file://' + smingdir
    else:
        basepath = baseurl + '/blob/' + get_github_rev()

    app.add_role('api-file', autolink(basepath + "/Sming/%s"))

    app.add_role('sample', doclink('/_inc/samples/%s/README'))
    app.add_role('component', doclink('/_inc/Sming/Components/%s/README'))
    app.add_role('component-esp8266', doclink('/_inc/Sming/Arch/Esp8266/Components/%s/README'))
    app.add_role('component-host', doclink('/_inc/Sming/Arch/Host/Components/%s/README'))
    app.add_role('library', doclink('/_inc/Sming/Libraries/%s/README'))

# Insert a link to a file in the repo
def autolink(pattern):
    def role(name, rawtext, text, lineno, inliner, options={}, content=[]):
        m = re.search('(.*)\s*<(.*)>', text)  # noqa: W605 - regular expression
        if m:
            link_text = m.group(1)
            link = m.group(2)
        else:
            link_text = text
            link = text
        url = pattern % (link,)
        node = nodes.reference(rawtext, link_text, refuri=url, **options)
        return [node], []
    return role


# Default title is extracted from target document
def doclink(pattern):
    def DocumentRole(typ, rawtext, text, lineno, inliner, options={}, content=[]):
        # type: (unicode, unicode, unicode, int, Inliner, Dict, List[unicode]) -> Tuple[List[nodes.Node], List[nodes.Node]]  # NOQA
        env = inliner.document.settings.env
        # split title and target in role content
        has_explicit_title, title, target = split_explicit_title(text)
        title = utils.unescape(title)
        target = pattern % (utils.unescape(target))
        # create the reference node
        refnode = addnodes.pending_xref(rawtext, reftype='doc', refdomain='std', refexplicit=has_explicit_title)
        # we may need the line number for warnings
        set_role_source_info(inliner, lineno, refnode)  # type: ignore
        # now that the target and title are finally determined, set them
        refnode['reftarget'] = target
        refnode += nodes.inline(rawtext, title, classes=['xref', 'doc'])
        # we also need the source document
        refnode['refdoc'] = env.docname
        refnode['refwarn'] = True
        return [refnode], []

    return DocumentRole
