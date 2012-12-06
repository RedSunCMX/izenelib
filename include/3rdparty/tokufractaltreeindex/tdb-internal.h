/* -*- mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*- */
// vim: ft=cpp:expandtab:ts=8:sw=4:softtabstop=4:
#ident "$Id: tdb-internal.h 46157 2012-07-25 20:49:56Z yfogel $"
#ifndef _TDB_INTERNAL_H
#define _TDB_INTERNAL_H

#ident "Copyright (c) 2007 Tokutek Inc.  All rights reserved."

#include "toku_list.h"
// Included by db.h, defines some internal structures.  These structures are inlined in some versions of db.h
// the types DB_TXN and so forth have been defined.

//// This list structure is repeated here (from toku_list.h) so that the db.h file will be standalone.  Any code that depends on this list matching the structure in toku_list.h
//// will get flagged by the compiler if someone changes one but not the other.   See #2276.
//struct toku_list {
//    struct toku_list *next, *prev;
//};

struct simple_dbt {
    uint32_t len;
    void     *data;
};

// end of _TDB_INTERNAL_H:
#endif
