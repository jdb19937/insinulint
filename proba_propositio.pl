#!/usr/bin/perl
# proba_propositio.pl — proba integrationis propositionum per omnes casus
#
# 1. Copia casus/ integrum in artificia/propositio/
# 2. Pro quoque .c, curre insinulint -p → .ddiff
# 3. Applica omnes .ddiff cum dpatch
# 4. Curre face in directorio copiato

use strict;
use warnings;

my $RADIX       = $0 =~ m{(.+)/} ? $1 : ".";
chomp($RADIX = `cd "$RADIX" && pwd`);
my $INSINULINT  = "$RADIX/insinulint";
my $DIR_CASUS   = "$RADIX/casus";
my $DIR_ARTIF   = "$RADIX/artificia/propositio";

my $VIRIDIS = "\033[0;32m";
my $RUBER   = "\033[0;31m";
my $FLAVUS  = "\033[0;33m";
my $NULLUS  = "\033[0m";

print "\n=== proba propositio ===\n\n";

# --- para: copia casus/ integrum ---
system("rm -rf $DIR_ARTIF");
system("mkdir -p $DIR_ARTIF");
system("cp -R $DIR_CASUS/* $DIR_ARTIF/");

my @plicae = sort glob("$DIR_ARTIF/*.c");
my $bonae  = 0;
my $malae  = 0;
my $vacuae = 0;

# --- genera ddiff pro quoque .c ---
for my $plica (@plicae) {
    (my $base = $plica) =~ s{.*/}{};
    $base =~ s{\.c$}{};
    my $ddiff_via = "$base.ddiff";

    system("cd $DIR_ARTIF && $INSINULINT -p $base.c > $ddiff_via 2>/dev/null");

    if (! -s "$DIR_ARTIF/$ddiff_via") {
        $vacuae++;
        printf "  ${FLAVUS}VACUUM${NULLUS} %s.c\n", $base;
        unlink "$DIR_ARTIF/$ddiff_via";
        next;
    }

    # applica ddiff
    my $rc = system("cd $DIR_ARTIF && dpatch $ddiff_via 2>$base.dpatch.err");
    if ($rc != 0) {
        open my $fh, '<', "$DIR_ARTIF/$base.dpatch.err";
        my $err = $fh ? do { local $/; <$fh> } : "";
        close $fh if $fh;
        $malae++;
        printf "  ${RUBER}MALUM${NULLUS} %s.c — dpatch fallit\n", $base;
        printf "    %s\n", $err if $err;
        next;
    }

    $bonae++;
    printf "  ${VIRIDIS}BONUM${NULLUS} %s.c\n", $base;
}

# --- purga cum face ---
system("face -C $DIR_ARTIF purga > /dev/null 2>&1");

# --- compila omnia cum face ---
print "\n--- compilatio ---\n";
my $rc_face = system("face -C $DIR_ARTIF 2>&1");
if ($rc_face != 0) {
    $malae++;
    printf "  ${RUBER}MALUM${NULLUS} face defecit\n";
} else {
    printf "  ${VIRIDIS}BONUM${NULLUS} face\n";
}

# --- summa ---
my $totae = scalar @plicae;
print "\n=== summa ===\n";
printf "totae: %d, bonae: %d, malae: %d, vacuae: %d\n",
    $totae, $bonae, $malae, $vacuae;
printf "artificia: %s/\n\n", $DIR_ARTIF;

if ($malae > 0) {
    printf "${RUBER}%d defectus.${NULLUS}\n", $malae;
    exit 1;
} else {
    printf "${VIRIDIS}Omnes probationes correctae.${NULLUS}\n";
    exit 0;
}
