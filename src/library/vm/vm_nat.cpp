/*
Copyright (c) 2016 Microsoft Corporation. All rights reserved.
Released under Apache 2.0 license as described in the file LICENSE.

Author: Leonardo de Moura
*/
#include <iostream>
#include "library/vm/vm.h"
#include "library/vm/vm_string.h"

namespace lean {
// =======================================
// Builtin nat operations

vm_obj mk_vm_nat(unsigned n) {
    if (n < LEAN_MAX_SMALL_NAT)
        return mk_vm_simple(n);
    else
        return mk_vm_mpz(mpz(n));
}

vm_obj mk_vm_nat(mpz const & n) {
    if (n < LEAN_MAX_SMALL_NAT)
        return mk_vm_simple(n.get_unsigned_int());
    else
        return mk_vm_mpz(n);
}

unsigned to_unsigned(vm_obj const & o) {
    if (is_simple(o))
        return cidx(o);
    else
        return to_mpz(o).get_unsigned_int();
}

optional<unsigned> try_to_unsigned(vm_obj const & o) {
    if (is_simple(o)) {
        return optional<unsigned>(cidx(o));
    } else {
        mpz const & v = to_mpz(o);
        if (v.is_unsigned_int())
            return optional<unsigned>(v.get_unsigned_int());
        else
            return optional<unsigned>();
    }
}

MK_THREAD_LOCAL_GET_DEF(mpz, get_mpz1);
MK_THREAD_LOCAL_GET_DEF(mpz, get_mpz2);

static mpz const & to_mpz1(vm_obj const & o) {
    if (is_simple(o)) {
        mpz & r = get_mpz1();
        r = cidx(o);
        return r;
    } else {
        return to_mpz(o);
    }
}

static mpz const & to_mpz2(vm_obj const & o) {
    if (is_simple(o)) {
        mpz & r = get_mpz2();
        r = cidx(o);
        return r;
    } else {
        return to_mpz(o);
    }
}

vm_obj nat_succ(vm_obj const & a) {
    if (is_simple(a)) {
        return mk_vm_nat(cidx(a) + 1);
    } else {
        return mk_vm_mpz(to_mpz1(a) + 1);
    }
}

vm_obj nat_add(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        return mk_vm_nat(cidx(a1) + cidx(a2));
    } else {
        return mk_vm_mpz(to_mpz1(a1) + to_mpz2(a2));
    }
}

vm_obj nat_mul(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        unsigned long long r = static_cast<unsigned long long>(cidx(a1)) * static_cast<unsigned long long>(cidx(a2));
        if (r < LEAN_MAX_SMALL_NAT) {
            return mk_vm_simple(r);
        }
    }
    return mk_vm_mpz(to_mpz1(a1) * to_mpz2(a2));
}

vm_obj nat_sub(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        unsigned v1 = cidx(a1);
        unsigned v2 = cidx(a2);
        if (v2 > v1)
            return mk_vm_simple(0);
        else
            return mk_vm_nat(v1 - v2);
    } else {
        mpz const & v1 = to_mpz1(a1);
        mpz const & v2 = to_mpz2(a2);
        if (v2 > v1)
            return mk_vm_simple(0);
        else
            return mk_vm_nat(v1 - v2);
    }
}

vm_obj nat_div(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        unsigned v1 = cidx(a1);
        unsigned v2 = cidx(a2);
        if (v2 == 0)
            return mk_vm_simple(0);
        else
            return mk_vm_nat(v1 / v2);
    } else {
        mpz const & v1 = to_mpz1(a1);
        mpz const & v2 = to_mpz2(a2);
        if (v2 == 0)
            return mk_vm_simple(0);
        else
            return mk_vm_nat(v1 / v2);
    }
}

vm_obj nat_mod(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        unsigned v1 = cidx(a1);
        unsigned v2 = cidx(a2);
        if (v2 == 0)
            return a1;
        else
            return mk_vm_nat(v1 % v2);
    } else {
        mpz const & v1 = to_mpz1(a1);
        mpz const & v2 = to_mpz2(a2);
        if (v2 == 0)
            return a1;
        else
            return mk_vm_nat(v1 % v2);
    }
}

vm_obj nat_gcd(vm_obj const & a1, vm_obj const & a2) {
    mpz r;
    gcd(r, to_mpz1(a1), to_mpz2(a2));
    return mk_vm_nat(r);
}

vm_obj nat_has_decidable_eq(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        return mk_vm_bool(cidx(a1) == cidx(a2));
    } else {
        return mk_vm_bool(to_mpz1(a1) == to_mpz2(a2));
    }
}

vm_obj nat_decidable_le(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        return mk_vm_bool(cidx(a1) <= cidx(a2));
    } else {
        return mk_vm_bool(to_mpz1(a1) <= to_mpz2(a2));
    }
}

vm_obj nat_decidable_lt(vm_obj const & a1, vm_obj const & a2) {
    if (is_simple(a1) && is_simple(a2)) {
        return mk_vm_bool(cidx(a1) < cidx(a2));
    } else {
        return mk_vm_bool(to_mpz1(a1) < to_mpz2(a2));
    }
}

void nat_rec(vm_state &) {
    /* recursors are implemented by the compiler */
    lean_unreachable();
}

void nat_no_confusion(vm_state &) {
    /* no_confusion is implemented by the compiler */
    lean_unreachable();
}

vm_obj nat_to_string(vm_obj const & a) {
    std::ostringstream out;
    if (is_simple(a)) {
        out << cidx(a);
    } else {
        out << to_mpz(a);
    }
    return to_obj(out.str());
}

vm_obj nat_repeat(vm_obj const &, vm_obj const & f, vm_obj const & n, vm_obj const & a) {
    if (is_simple(n)) {
        unsigned _n = cidx(n);
        vm_obj   r  = a;
        for (unsigned i = 0; i < _n ; i++) {
            r = invoke(f, mk_vm_simple(i), r);
        }
        return r;
    } else {
        mpz _n = to_mpz(n);
        mpz i(0);
        vm_obj r = a;
        while (i < _n) {
            r = invoke(f, mk_vm_nat(i), r);
            i++;
        }
        return r;
    }
}

void initialize_vm_nat() {
    DECLARE_VM_BUILTIN(name({"nat", "succ"}),             nat_succ);
    DECLARE_VM_BUILTIN(name({"nat", "add"}),              nat_add);
    DECLARE_VM_BUILTIN(name({"nat", "mul"}),              nat_mul);
    DECLARE_VM_BUILTIN(name({"nat", "sub"}),              nat_sub);
    DECLARE_VM_BUILTIN(name({"nat", "div"}),              nat_div);
    DECLARE_VM_BUILTIN(name({"nat", "mod"}),              nat_mod);
    DECLARE_VM_BUILTIN(name({"nat", "gcd"}),              nat_gcd);
    DECLARE_VM_BUILTIN(name({"nat", "has_decidable_eq"}), nat_has_decidable_eq);
    DECLARE_VM_BUILTIN(name({"nat", "decidable_le"}),     nat_decidable_le);
    DECLARE_VM_BUILTIN(name({"nat", "decidable_lt"}),     nat_decidable_lt);
    DECLARE_VM_BUILTIN(name({"nat", "to_string"}),        nat_to_string);
    DECLARE_VM_BUILTIN(name({"nat", "repeat"}),           nat_repeat);

    declare_vm_builtin(name({"nat", "cases_on"}),          "nat_rec",          4, nat_rec);
    declare_vm_builtin(name({"nat", "rec_on"}),            "nat_rec",          4, nat_rec);
    declare_vm_builtin(name({"nat", "no_confusion"}),      "nat_no_confusion", 5, nat_no_confusion);
    declare_vm_builtin(name({"nat", "no_confusion_type"}), "nat_no_confusion", 3, nat_no_confusion);
}

void finalize_vm_nat() {
}
}
