/*
 * correctio.h — correctio plicarum C in loco
 *
 * Corriges plicam in loco: indentationem, spatia terminalia,
 * lineas vacuas, finem lineae novum.
 */

#ifndef CORRECTIO_H
#define CORRECTIO_H

#include "insinulint.h"

/*
 * correctio_age — corriges plicam in loco ex speculo.
 * reddit 0 si bene, -1 si error.
 */
int correctio_age(const char *via, const speculum_t *spec);

/*
 * correctio_excommenta — exstirpat omnia commentaria ex plica.
 * reddit 0 si bene, -1 si error.
 */
int correctio_excommenta(const char *via);

#endif /* CORRECTIO_H */
