#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512
#define NPIPES 208

const char *pipes[NPIPES] = {
  "TU1",
  "TU10",
  "TU100",
  "TU101",
  "TU102",
  "TU103",
  "TU104",
  "TU105",
  "TU106",
  "TU107",
  "TU108",
  "TU109",
  "TU11",
  "TU110",
  "TU111",
  "TU112",
  "TU113",
  "TU114",
  "TU115",
  "TU116",
  "TU117",
  "TU118",
  "TU119",
  "TU12",
  "TU120",
  "TU121",
  "TU122",
  "TU123",
  "TU124",
  "TU125",
  "TU126",
  "TU127",
  "TU128",
  "TU129",
  "TU13",
  "TU130",
  "TU131",
  "TU132",
  "TU133",
  "TU134",
  "TU135",
  "TU136",
  "TU137",
  "TU138",
  "TU139",
  "TU14",
  "TU140",
  "TU141",
  "TU142",
  "TU143",
  "TU144",
  "TU145",
  "TU146",
  "TU147",
  "TU148",
  "TU149",
  "TU15",
  "TU150",
  "TU151",
  "TU152",
  "TU15301",
  "TU15302",
  "TU1074",
  "TU154",
  "TU155",
  "TU156",
  "TU157",
  "TU158",
  "TU159",
  "TU16",
  "TU160",
  "TU161",
  "TU162",
  "TU163",
  "TU164",
  "TU166",
  "TU167",
  "TU168",
  "TU169",
  "TU17",
  "TU170",
  "TU171",
  "TU17201",
  "TU17202",
  "TU1011A",
  "TU173",
  "TU174",
  "TU175",
  "TU176",
  "TU177",
  "TU178",
  "TU179",
  "TU18",
  "TU180",
  "TU181",
  "TU182",
  "TU183",
  "TU184",
  "TU185",
  "TU186",
  "TU187",
  "TU188",
  "TU189",
  "TU19",
  "TU190",
  "TU191",
  "TU192",
  "TU193",
  "TU194",
  "TU195",
  "TU196",
  "TU197",
  "TU198",
  "TU199",
  "TU2",
  "TU20",
  "TU200",
  "TU201",
  "TU203",
  "TU204",
  "TU205",
  "TU21",
  "TU22",
  "TU23",
  "TU24",
  "TU25",
  "TU26",
  "TU27",
  "TU28",
  "TU29",
  "TU3",
  "TU30",
  "TU31",
  "TU32",
  "TU33",
  "TU34",
  "TU35",
  "TU36",
  "TU37",
  "TU38",
  "TU39",
  "TU4",
  "TU40",
  "TU41",
  "TU42",
  "TU43",
  "TU44",
  "TU45",
  "TU46",
  "TU47",
  "TU48",
  "TU49",
  "TU5",
  "TU50",
  "TU52",
  "TU53",
  "TU54",
  "TU55",
  "TU56",
  "TU57",
  "TU58",
  "TU59",
  "TU6",
  "TU61",
  "TU62",
  "TU63",
  "TU65",
  "TU66",
  "TU67",
  "TU68",
  "TU69",
  "TU7",
  "TU70",
  "TU71",
  "TU72",
  "TU73",
  "TU74",
  "TU75",
  "TU76",
  "TU77",
  "TU78",
  "TU79",
  "TU8",
  "TU80",
  "TU81",
  "TU82",
  "TU83",
  "TU84",
  "TU85",
  "TU86",
  "TU87",
  "TU88",
  "TU89",
  "TU9",
  "TU90",
  "TU91",
  "TU92",
  "TU93",
  "TU94",
  "TU95",
  "TU96",
  "TU97",
  "TU98",
  "TU99",
  "TU4000",
  "TU60",
  "TU64",
  "TU51"
};

int
main (int argn, char **argc)
{
  char buffer[BUFFER_SIZE], name[BUFFER_SIZE];
  FILE *file;
  const char *id;
  double second;
  unsigned int i, year, month, day, hour, minute;
  if (argn != 7)
    {
      printf ("The syntax is:\n./results year month day hour minute second\n");
      return 1;
    }
  year = atoi (argc[1]);
  month = atoi (argc[2]);
  day = atoi (argc[3]);
  hour = atoi (argc[4]);
  minute = atoi (argc[5]);
  second = atof (argc[6]);
  snprintf (name, BUFFER_SIZE, "%u-%u-%u-%u-%u-%lg",
            year, month, day, hour, minute, second);
  file = fopen ("r.xml", "w");
  fprintf (file, "<?xml version=\"1.0\"?>\n");
  fprintf (file, "<results>\n");
  for (i = 0; i < NPIPES; ++i)
    {
      id = pipes[i];
      fprintf (file, "\t<pipe id=\"%s\" time=\"%u %u %u %u %u %lg\" "
               "file=\"results-pipe-%s-%s\"/>\n",
               id, year, month, day, hour, minute, second, id, name);
    }
  fputs ("</results>", file);
  fclose (file);
  system ("./zebra results.bin r.xml");
  snprintf (buffer, BUFFER_SIZE, "for i in results-pipe-*-%s; "
            "do ./translate-csv $i $i.csv; done", name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "echo \"Line\tID\tpatternID\tz\tCl\tH2O2\t"
            "OrganicMatter\tO2\tlarvae\tadults\tmass\" > results-%s.csv", name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "cat results-pipe-*-%s.csv >> results-%s.csv",
            name, name);
  system (buffer);
  snprintf (buffer, BUFFER_SIZE, "rm results-pipe-*-%s*", name);
  system (buffer);
  return 0;
}
