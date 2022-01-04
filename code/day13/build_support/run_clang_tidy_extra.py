#!/usr/bin/env python

"""
A helper class, to suppress execution of clang-tidy.

In clang-tidy-6.0, if the clang-tidy configuration file suppresses ALL checks,
(e.g. via a .clang-tidy file), clang-tidy will print usage information and
exit with a return code of 0. Harmless but verbose. In later versions of
clang-tidy the return code becomes 1, making this a bigger problem.

This helper addresses the problem by suppressing execution according to
the configuration in this file.
"""

import re

class CheckConfig(object):
    """ Check paths against the built-in config """

    def __init__(self):
        self._init_config()
        # debug prints
        self.debug = False
        return

    def _init_config(self):
        """ Any path matching one of the ignore_pats regular expressions,
            denotes that we do NOT want to run clang-tidy on that item.
        """
        self.ignore_pats = [".*/third_party/.*", ]
        return

    def should_skip(self, path):
        """ Should execution of clang-tidy be skipped?
            path - to check, against the configuration.
                   Typically the full path.
            returns - False if we want to run clang-tidy
                      True of we want to skip execution on this item
        """
        for pat in self.ignore_pats:
            if re.match(pat, path):
                if self.debug:
                    print("match pat: {}, {} => don't run".format(pat, path))
                return True
        return False
