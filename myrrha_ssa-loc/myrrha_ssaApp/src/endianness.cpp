#include <registryFunction.h>
#include <epicsExport.h>
#include <dbAccessDefs.h>
#include <aSubRecord.h>
#include <algorithm>
#include <cstdio>

struct FieldInfo {
    void *field;
    epicsEnum16 type;
    epicsUInt32 len;
};

static long copyBytes(struct aSubRecord *rec, bool reverse) {
    static constexpr size_t numFields = 21;
    FieldInfo const inputs[numFields] = {
        {rec->a, rec->fta, rec->nea},
        {rec->b, rec->ftb, rec->neb},
        {rec->c, rec->ftc, rec->nec},
        {rec->d, rec->ftd, rec->ned},
        {rec->e, rec->fte, rec->nee},
        {rec->f, rec->ftf, rec->nef},
        {rec->g, rec->ftg, rec->neg},
        {rec->h, rec->fth, rec->neh},
        {rec->i, rec->fti, rec->nei},
        {rec->j, rec->ftj, rec->nej},
        {rec->k, rec->ftk, rec->nek},
        {rec->l, rec->ftl, rec->nel},
        {rec->m, rec->ftm, rec->nem},
        {rec->n, rec->ftn, rec->nen},
        {rec->o, rec->fto, rec->neo},
        {rec->p, rec->ftp, rec->nep},
        {rec->q, rec->ftq, rec->neq},
        {rec->r, rec->ftr, rec->ner},
        {rec->s, rec->fts, rec->nes},
        {rec->t, rec->ftt, rec->net},
        {rec->u, rec->ftu, rec->neu},
    };
    FieldInfo const outputs[numFields] = {
        {rec->vala, rec->ftva, rec->neva},
        {rec->valb, rec->ftvb, rec->nevb},
        {rec->valc, rec->ftvc, rec->nevc},
        {rec->vald, rec->ftvd, rec->nevd},
        {rec->vale, rec->ftve, rec->neve},
        {rec->valf, rec->ftvf, rec->nevf},
        {rec->valg, rec->ftvg, rec->nevg},
        {rec->valh, rec->ftvh, rec->nevh},
        {rec->vali, rec->ftvi, rec->nevi},
        {rec->valj, rec->ftvj, rec->nevj},
        {rec->valk, rec->ftvk, rec->nevk},
        {rec->vall, rec->ftvl, rec->nevl},
        {rec->valm, rec->ftvm, rec->nevm},
        {rec->valn, rec->ftvn, rec->nevn},
        {rec->valo, rec->ftvo, rec->nevo},
        {rec->valp, rec->ftvp, rec->nevp},
        {rec->valq, rec->ftvq, rec->nevq},
        {rec->valr, rec->ftvr, rec->nevr},
        {rec->vals, rec->ftvs, rec->nevs},
        {rec->valt, rec->ftvt, rec->nevt},
        {rec->valu, rec->ftvu, rec->nevu},
    };

    for (uint i = 0; i < numFields; ++i) {
        auto &in = inputs[i];
        auto &out = outputs[i];
        if (in.field != nullptr && out.field != nullptr) {
            size_t const numBytesIn = in.len * dbValueSize(in.type);
            size_t const numBytesOut = out.len * dbValueSize(out.type);

            if (numBytesIn != numBytesOut) {
                printf("%s: mismatching sizes on link %d\n", rec->name, i);
                return 1;
            }

            auto din = static_cast<char const*>(in.field);
            auto dout = static_cast<char*>(out.field);
            if (reverse) {
                std::reverse_copy(din, din + numBytesIn, dout);
            } else {
                std::copy(din, din + numBytesIn, dout);
            }
        }
    }

    return 0;
}

static long reverseBytes(struct aSubRecord *rec) {
    return copyBytes(rec, true);
}

static long fromBigEndian(struct aSubRecord *rec) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    bool reverse = true;
#else
    bool reverse = false;
#endif
    return copyBytes(rec, reverse);
}

static long fromLittleEndian(struct aSubRecord *rec) {
#if __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
    bool reverse = true;
#else
    bool reverse = false;
#endif
    return copyBytes(rec, reverse);
}

epicsRegisterFunction(reverseBytes);
epicsRegisterFunction(fromBigEndian);
epicsRegisterFunction(fromLittleEndian);
