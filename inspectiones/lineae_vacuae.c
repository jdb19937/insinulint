/*
 * inspectiones/lineae_vacuae.c — lineae vacuae consecutivae
 */

#include "../inspectio.h"

#include <stdio.h>

void inspice_lineas_vacuas(
    inspector_t *ins,
    const versus_t *versus, int num_versus,
    int maxima
) {
    if (maxima <= 0)
        return;
    int vacuae = 0;
    for (int vi = 0; vi < num_versus; vi++) {
        if (versus_vacuus(&versus[vi])) {
            vacuae++;
        } else {
            if (vacuae > maxima) {
                char nuntius[NUNTIUS_MAX];
                snprintf(
                    nuntius, sizeof(nuntius),
                    "%d lineae vacuae consecutivae (max %d)",
                    vacuae, maxima
                );
                inspector_adde(
                    ins, GRAVITAS_MONITUM,
                    versus[vi].numero, 0,
                    "lineae_vacuae", nuntius
                );
            }
            vacuae = 0;
        }
    }
}
