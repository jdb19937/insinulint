#!/usr/bin/perl
# proba_formae.pl — proba idempotentiae correctionum per omnes formas et casus
#
# Pro unaquaque forma:
#   1. Copia casus/ in artificia/forma.1/, inspice (.0.txt), corrige, inspice (.1.txt)
#   2. Copia forma.1/ in forma.2/, corrige, inspice (.2.txt)
#   3. Compara .c inter forma.1/ et forma.2/ — idempotentia
#   4. Aedifica forma.1/ et forma.2/ per face — compilatio

use strict;
use warnings;

my $RADIX       = $0 =~ m{(.+)/} ? $1 : ".";
chomp($RADIX = `cd "$RADIX" && pwd`);
my $INSINULINT  = "$RADIX/insinulint";
my $DIR_CASUS   = "$RADIX/casus";
my $DIR_FORMAE  = "$RADIX/formae";
my $DIR_ARTIF   = "$RADIX/artificia/formae";

my $VIRIDIS = "\033[0;32m";
my $FLAVUS  = "\033[0;33m";
my $RUBER   = "\033[0;31m";
my $NULLUS  = "\033[0m";

sub curre {
    my ($mandatum, $via_rc) = @_;
    system($mandatum);
    my $rc = $? >> 8;
    if ($via_rc) {
        open my $fh, '>', $via_rc or die "Non potest scribere $via_rc: $!\n";
        printf $fh "%d\n", $rc;
        close $fh;
    }
    return $rc;
}

sub numera_lineas {
    my ($via) = @_;
    open my $fh, '<', $via or return 0;
    my $n = 0;
    $n++ while <$fh>;
    close $fh;
    return $n;
}

my $defectus_idem   = 0;
my $defectus_corr   = 0;
my $defectus_compil = 0;
my $summa           = 0;

# --- aedifica insinulint et casus originales ---
print "=== Aedificatio ===\n";
curre("face -C $RADIX");
curre("face -C $DIR_CASUS");
print "Aedificatio perfecta.\n\n";

system("rm -rf $DIR_ARTIF");
system("mkdir -p $DIR_ARTIF");

my @formae = sort glob("$DIR_FORMAE/*.ison");

sub nomen_base {
    my ($via, $ext) = @_;
    (my $n = $via) =~ s{.*/}{};
    $n =~ s{\Q$ext\E$}{} if $ext;
    return $n;
}

sub compara_plicas {
    my ($a, $b) = @_;
    open my $fa, '<', $a or return 1;
    open my $fb, '<', $b or return 1;
    local $/;
    my $res = (<$fa> eq <$fb>) ? 0 : 1;
    close $fa; close $fb;
    return $res;
}

if (@ARGV) {
    my %electae = map { $_ => 1 } @ARGV;
    @formae = grep { $electae{nomen_base($_, ".ison")} } @formae;
    die "Nullae formae inventae pro: @ARGV\n" unless @formae;
}

for my $forma (@formae) {
    my $nomen_formae = nomen_base($forma, ".ison");
    my $dir1 = "$DIR_ARTIF/${nomen_formae}.1";
    my $dir2 = "$DIR_ARTIF/${nomen_formae}.2";
    printf "--- %s ---\n", $nomen_formae;

    # passus 1: copia casus/, inspice, corrige, inspice
    curre("cp -R $DIR_CASUS $dir1");
    my @plicae1 = sort glob("$dir1/*.c");

    for my $plica1 (@plicae1) {
        my $base = nomen_base($plica1, ".c");
        curre("$INSINULINT -s $forma $plica1 >/dev/null 2>$dir1/${base}.0.txt",
              "$dir1/${base}.0.rc");
    }
    for my $plica1 (@plicae1) {
        my $base = nomen_base($plica1, ".c");
        my $rc = curre("$INSINULINT -s $forma -c $plica1 >/dev/null 2>&1",
                        "$dir1/${base}.c.rc");
        die "Correctio defecit (exit $rc): $plica1\n" if $rc != 0;
    }
    for my $plica1 (@plicae1) {
        my $base = nomen_base($plica1, ".c");
        curre("$INSINULINT -s $forma $plica1 >/dev/null 2>$dir1/${base}.1.txt",
              "$dir1/${base}.1.rc");
    }

    # passus 2: copia dir1/, corrige, inspice
    curre("cp -R $dir1 $dir2");
    my @plicae2 = sort glob("$dir2/*.c");

    for my $plica2 (@plicae2) {
        my $base = nomen_base($plica2, ".c");
        my $rc = curre("$INSINULINT -s $forma -c $plica2 > /dev/null 2>&1",
                        "$dir2/${base}.c.rc");
        die "Correctio defecit (exit $rc): $plica2\n" if $rc != 0;
    }
    for my $plica2 (@plicae2) {
        my $base = nomen_base($plica2, ".c");
        curre("$INSINULINT -s $forma $plica2 >/dev/null 2>$dir2/${base}.2.txt",
              "$dir2/${base}.2.rc");
    }

    # confirma idempotentiam: compara .c inter dir1 et dir2
    for my $plica1 (@plicae1) {
        my $base = nomen_base($plica1, ".c");
        my $plica2 = "$dir2/${base}.c";
        my $n0 = numera_lineas("$dir1/${base}.0.txt");
        my $n1 = numera_lineas("$dir1/${base}.1.txt");
        my $n2 = numera_lineas("$dir2/${base}.2.txt");

        my $rc1_via = "$dir1/${base}.1.rc";
        my $rc2_via = "$dir2/${base}.2.rc";
        open my $fh1, '<', $rc1_via or die "Non potest legere $rc1_via: $!\n";
        my $rc1 = <$fh1> + 0; close $fh1;
        open my $fh2, '<', $rc2_via or die "Non potest legere $rc2_via: $!\n";
        my $rc2 = <$fh2> + 0; close $fh2;

        $summa++;
        my $idempotens = (compara_plicas($plica1, $plica2) == 0);
        my @def_idem;
        my @def_corr;
        push @def_idem, "non idempotens" if !$idempotens;
        push @def_idem, "monita mutata ($n1 -> $n2)" if $n1 != $n2;
        push @def_corr, "monita residua" if $n1 != 0;

        if (@def_idem) {
            printf " ${RUBER}DEFECTUS${NULLUS} [%s × %s] monita: %d -> %d -> %d — %s\n",
                $nomen_formae, $base, $n0, $n1, $n2, join(", ", @def_idem);
            $defectus_idem++;
        } elsif (@def_corr) {
            printf " ${RUBER}DEFECTUS${NULLUS} [%s × %s] monita: %d -> %d -> %d — %s\n",
                $nomen_formae, $base, $n0, $n1, $n2, join(", ", @def_corr);
            $defectus_corr++;
        } else {
            printf "${VIRIDIS}CORRECTUS${NULLUS} [%s × %s] monita: %d -> %d -> %d\n",
                $nomen_formae, $base, $n0, $n1, $n2;
        }
    }

    # confirma compilationem per face in utroque directorio
    for my $dir ($dir1, $dir2) {
        my $tag = nomen_base($dir);
        my $rc = system("face -C $dir >/dev/null 2>&1");
        if ($rc != 0) {
            printf " ${RUBER}DEFECTUS${NULLUS} [%s] — face defecit\n", $tag;
            $defectus_compil++;
        } else {
            printf "${VIRIDIS}CORRECTUS${NULLUS} [%s] — face\n", $tag;
        }
        system("face -C $dir purga >/dev/null 2>&1");
    }
}

# --- summa ---
my $defectus = $defectus_idem + $defectus_compil;
print "\n=== Summa ===\n";
printf "Probationes:          %d\n", $summa;
printf "Defectus idempotent.: %d\n", $defectus_idem;
printf "Defectus correctio.:  %d\n", $defectus_corr;
printf "Defectus compilat.:   %d\n", $defectus_compil;
printf "Artificia:            %s/\n", $DIR_ARTIF;

if ($defectus > 0) {
    printf "${RUBER}%d defectus inventi.${NULLUS}\n", $defectus;
    exit 1;
} elsif ($defectus_corr > 0) {
    printf "${RUBER}%d correctiones imperfectae.${NULLUS}\n", $defectus_corr;
    exit 0;
} else {
    printf "${VIRIDIS}Omnes probationes correctae.${NULLUS}\n";
    exit 0;
}
