#!/usr/bin/perl
#
# Этот скрипт изменил названия схем и регионов в файле perl.hrc
# 



use strict;

my $flin= shift(@ARGV);
my $text;

unless(open(XMLIN,"$flin")) 
{
 die("File $flin not foind. Transformation failed.\n");
}
while(<XMLIN>)
{
 $text.=$_;
}
close(XMLIN);

$text =~ s/(region\d{0,2}|parent|scheme)\s*=\s*"(?!pl|perl|def:|regexp:)([\w:._-]+)"/$1="def:$2"/gs;
$text =~ s/(name|region\d{0,2}|parent|scheme)\s*=\s*"(perl|pl)([\w]+)"/$1="$3"/gs;

print $text;
