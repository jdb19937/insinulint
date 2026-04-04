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
use File::Basename;
use File::Copy;
use File::Path qw(make_path remove_tree);
use File::Compare;
use File::Find;
use Cwd qw(abs_path);

my $RADIX       = dirname(abs_path($0));
my $INSINULINT  = "$RADIX/insinulint";
my $DIR_CASUS   = "$RADIX/casus";
my $DIR_FORMAE  = "$RADIX/formae";
my $DIR_ARTIF   = "$RADIX/artificia";

my $VIRIDIS = "\033[0;32m";
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
my $defectus_compil = 0;
my $summa           = 0;

# --- aedifica insinulint et casus originales ---
print "=== Aedificatio ===\n";
curre("face -C $RADIX");
curre("face -C $DIR_CASUS");
print "Aedificatio perfecta.\n\n";

remove_tree($DIR_ARTIF);
make_path($DIR_ARTIF);

my @formae = sort glob("$DIR_FORMAE/*.ison");

if (@ARGV) {
    my %electae = map { $_ => 1 } @ARGV;
    @formae = grep { $electae{basename($_, ".ison")} } @formae;
    die "Nullae formae inventae pro: @ARGV\n" unless @formae;
}

for my $forma (@formae) {
    my $nomen_formae = basename($forma, ".ison");
    my $dir1 = "$DIR_ARTIF/${nomen_formae}.1";
    my $dir2 = "$DIR_ARTIF/${nomen_formae}.2";
    printf "--- %s ---\n", $nomen_formae;

    # passus 1: copia casus/, inspice, corrige, inspice
    curre("cp -R $DIR_CASUS $dir1");
    my @plicae1 = sort glob("$dir1/*.c");

    for my $plica1 (@plicae1) {
        my $base = basename($plica1, ".c");
        curre("$INSINULINT -s $forma $plica1 >/dev/null 2>$dir1/${base}.0.txt",
              "$dir1/${base}.0.rc");
    }
    for my $plica1 (@plicae1) {
        my $base = basename($plica1, ".c");
        my $rc = curre("$INSINULINT -s $forma -c $plica1 >/dev/null 2>&1",
                        "$dir1/${base}.c.rc");
        die "Correctio defecit (exit $rc): $plica1\n" if $rc != 0;
    }
    for my $plica1 (@plicae1) {
        my $base = basename($plica1, ".c");
        curre("$INSINULINT -s $forma $plica1 >/dev/null 2>$dir1/${base}.1.txt",
              "$dir1/${base}.1.rc");
    }

    # passus 2: copia dir1/, corrige, inspice
    curre("cp -R $dir1 $dir2");
    my @plicae2 = sort glob("$dir2/*.c");

    for my $plica2 (@plicae2) {
        my $base = basename($plica2, ".c");
        my $rc = curre("$INSINULINT -s $forma -c $plica2 > /dev/null 2>&1",
                        "$dir2/${base}.c.rc");
        die "Correctio defecit (exit $rc): $plica2\n" if $rc != 0;
    }
    for my $plica2 (@plicae2) {
        my $base = basename($plica2, ".c");
        curre("$INSINULINT -s $forma $plica2 >/dev/null 2>$dir2/${base}.2.txt",
              "$dir2/${base}.2.rc");
    }

    # confirma idempotentiam: compara .c inter dir1 et dir2
    for my $plica1 (@plicae1) {
        my $base = basename($plica1, ".c");
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
        my $idempotens = (compare($plica1, $plica2) == 0);
        my @defectus;
        push @defectus, "non idempotens" if !$idempotens;
        push @defectus, "monita residua" if $n1 != 0 || $n2 != 0;
        push @defectus, "exit $rc1 post corr." if $rc1 != 0;
        push @defectus, "exit $rc2 post corr. 2" if $rc2 != 0;

        if (@defectus) {
            printf "${RUBER}DEFECTUS${NULLUS} [%s × %s] monita: %d -> %d -> %d — %s\n",
                $nomen_formae, $base, $n0, $n1, $n2, join(", ", @defectus);
            $defectus_idem++;
        } else {
            printf "${VIRIDIS}BENE${NULLUS}    [%s × %s] monita: %d -> %d -> %d\n",
                $nomen_formae, $base, $n0, $n1, $n2;
        }
    }

    # confirma compilationem per face in utroque directorio
    for my $dir ($dir1, $dir2) {
        my $tag = basename($dir);
        my $rc = system("face -C $dir >/dev/null 2>&1");
        if ($rc != 0) {
            printf "${RUBER}DEFECTUS${NULLUS} [%s] — face defecit\n", $tag;
            $defectus_compil++;
        } else {
            printf "${VIRIDIS}BENE${NULLUS}    [%s] — face\n", $tag;
        }
        system("face -C $dir purga >/dev/null 2>&1");
    }
}

# --- summa ---
my $defectus = $defectus_idem + $defectus_compil;
print "\n=== Summa ===\n";
printf "Probationes:          %d\n", $summa;
printf "Defectus idempotent.: %d\n", $defectus_idem;
printf "Defectus compilat.:   %d\n", $defectus_compil;
printf "Artificia:            %s/\n", $DIR_ARTIF;

if ($defectus > 0) {
    printf "${RUBER}%d defectus inventi.${NULLUS}\n", $defectus;
    exit 1;
} else {
    printf "${VIRIDIS}Omnes probationes praeterierunt.${NULLUS}\n";
    exit 0;
}
