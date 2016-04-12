#!/usr/bin/perl

open(PARSER,"Parser.cpp");
open(NP,">ParserNew.cpp");

$type=0;

while (<PARSER>) {
      $type=1 if (/::Erorr/ || /::SynErr/);
      $type=2 if (/::Warning/);
      $type=3 if (/::Exception/);	
      if (/wprintf/) {
      	 print NP "    wchar_t buffer[1024];\n";
         /wprintf\(([^)]+)\)/;
	 print NP "    swprintf(buffer,1024,$1);\n";
	 print NP "    char cbuffer[1024];\n";
         print NP "    memset(cbuffer,0,1024);\n";
	 print NP "    for (int i=0; i<1024 && buffer[i]!=0; i++) cbuffer[i]=wctob(buffer[i]);\n";	 
	 print NP "    LOG4CPLUS_";
	 if ($type==1) { print NP "ERROR"}
	 if ($type==2) { print NP "WARN"}
	 if ($type==3) { print NP "FATAL"}
	 print NP "(log4cplus::Logger::getInstance(\"Hcal.CfgScriptParser\"),cbuffer);\n";
      } elsif (/\\n/) {
	  s/\\n//;
	  print NP;
      } else {
	  print NP;
      }
}
close (PARSER);
close (NP);
system("mv Parser.cpp ParserOrig.cpp");
system("mv ParserNew.cpp Parser.cpp");
