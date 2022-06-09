#!/usr/bin/perl -w
use CGI;

$upload_dir = $ENV{'UPLOAD_FILE_PATH'};

$query = new CGI;

$filename = $query->param("uploadedFile");
$filename =~ s/.*[\/\\](.*)/$1/;
$upload_filehandle = $query->upload("uploadedFile");

open UPLOADFILE, ">$upload_dir/$filename";

while ( <$upload_filehandle> )
{
print UPLOADFILE;
}

close UPLOADFILE;

print $query->header ( );
print <<END_HTML;

<HTML>
<HEAD>
<TITLE>Thanks!</TITLE>
</HEAD>

<BODY>

<P>Thanks for uploading your photo!</P>

</BODY>
</HTML>

END_HTML