
$path4ever = "../../../colorer.4ever/";
$pathtake5 = "../../";

$mask = '\b.+\.hr[dc]';

mkdir $pathtake5."hrc", 0777;
mkdir $pathtake5."hrd", 0777;

parseRecurse($path4ever."hrd/");
parseRecurse($path4ever."hrc/");


sub parseRecurse{
 local($dr) = @_[0];
 foreach(`ls -1 $dr`){
   chomp;
   if (-d $dr.$_){
     $path = $dr;
     parseRecurse($path.$_."/");
   }else{
     next if (!/$mask/);
     $path = $dr;
     $path =~ s/$path4ever/$pathtake5/;
     if (/\.hrd$/){
       print  "xslt.bat $dr$_ 4ever2take5.hrd.xsl $path$_\n";
       system "xslt.bat $dr$_ 4ever2take5.hrd.xsl $path$_";
     }elsif (/\.hrc$/){
       print  "xslt.bat $dr$_ 4ever2take5.hrc.xsl $path$_\n";
       system "xslt.bat $dr$_ 4ever2take5.hrc.xsl $path$_";
     };
   };
 };
};
