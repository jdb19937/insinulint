/*
 * correctio.h — caput correctionum
 *
 * Includit omnia sub-capita correctionum.
 */

#ifndef CORRECTIO_H
#define CORRECTIO_H

#include "correctiones/communes.h"
#include "correctiones/corpus.h"
#include "correctiones/excommenta.h"
#include "correctiones/cubitus.h"
#include "correctiones/colineatio.h"
#include "correctiones/apertio.h"
#include "correctiones/scissio.h"
#include "correctiones/virgula.h"
#include "correctiones/operatores.h"
#include "correctiones/verbum.h"
#include "correctiones/semicolon.h"
#include "correctiones/longitudo.h"
#include "correctiones/una_sententia.h"
#include "correctiones/bracchia.h"

#include <string.h>

/* ================================================================
 * interfacies publica
 * ================================================================ */

int correctio_age(const char *via, const speculum_t *spec);

#endif /* CORRECTIO_H */
