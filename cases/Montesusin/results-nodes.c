#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512
#define NNODES 209º
const char *nodes[NNODES] =
  { "3661", "482", "661", "6610", "6611", "6612"  "6613", "6615",
  "6616", "6617", "6618", "662", "6620", "6621", "6622", "6623",
  "6624", "6625", "6626",
  "6627", "6628", "6629", "663", "6630", "6632", "6633", "6634",
  "6635", "6636", "6637",
  "6639", "664", "6640", "6641", "6642", "6643", "6644", "6645",
  "6646", "6648", "6649",
  "665", "666", "6665", "6666", "667", "668", "669", "1", "10", "100",
  "100B", "101", "103",
  "104", "105", "106", "107", "108", "109", "11B", "110", "111",
  "112", "113", "114", "115",
  "116", "117", "118", "119", "12", "121", "122", "123", "125", "126",
  "127", "128", "129",
  "130", "130B", "132", "134", "135", "137", "139", "14", "140",
  "141", "142", "143", "144",
  "146", "147", "148", "149", "15", "150", "151", "152", "153", "154",
  "157", "158A",
  "158B", "16B", "160", "162", "163", "164", "165", "166", "167",
  "169", "17A", "170",
  "17B", "171", "172", "18B", "19", "2", "20", "2002", "2003", "21",
  "22", "222", "24",
  "25", "26", "27", "28", "29", "3", "30", "3002", "3004", "3005",
  "3008", "3012", "3013",
  "3016", "3017", "3018", "3019", "3020", "3024", "31", "3152", "32",
  "34", "35", "36",
  "39", "41", "46", "47", "48", "49", "5", "52", "53", "54", "55",
  "56", "58", "59", "6",
  "60", "61", "62", "64", "65", "66", "67", "68", "69", "7", "70",
  "71", "72", "73", "77",
  "78", "79", "8", "80", "81", "82", "85", "86", "87", "88", "89",
  "90", "91", "92",
  "91B", "94", "95", "97", "99", "74", "1074", "11A", "1011", "0" 
};

 int 
main (int argn, char **argc)  { char buffer[BUFFER_SIZE];
  FILE * file;
  const char *id;
  double second;
  unsigned int i;
  if (argn != 2)
    
    {
      printf ("The syntax is:\n./results-nodes results_file\n");
      return 1;
    }
  printf ("Doing configuration file\n");
  file = fopen ("r.xml", "w");
  fprintf (file, "<?xml version=\"1.0\"?>\n");
  fprintf (file, "<results>\n");
  for (i = 0; i < NNODES; ++i)
    
    {
      id = nodes[i];
      fprintf (file, "\t<pipe id=\"%s\" time=\"%u %u %u %u %u %lg\" " 
                "file=\"%s-pipe-%s-%s\"/>\n", id, year, month, day, hour,
                minute, second, argc[1], id, name);
    }
  fputs ("</results>", file);
  fclose (file);
  printf ("Executing zebra\n");
  snprintf (buffer, BUFFER_SIZE, "./zebra %s r.xml", argc[1]);
  system (buffer);
  printf ("Translating every file to .csv\n");
  snprintf (buffer, BUFFER_SIZE, "for i in %s-pipe-*-%s; " 
             "do ./translate-csv $i $i.csv; done", argc[1], name);
  system (buffer);
  printf ("Doing final .csv file\n");
  snprintf (buffer, BUFFER_SIZE, "echo \"Line\tID\tpatternID\tz\tCl\tH2O2\t" 
             "OrganicMatter\tO2\tlarvae\tadults\tmass\" > %s-%s.csv", argc[1],
             name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "cat %s-pipe-*-%s.csv >> %s-%s.csv", argc[1],
             name, argc[1], name);
  system (buffer);
  printf ("Removing temporal files\n");
  snprintf (buffer, BUFFER_SIZE, "rm %s-pipe-*-%s*", argc[1], name);
  system (buffer);
  return 0;
}


