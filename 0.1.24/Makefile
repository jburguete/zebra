.PHONY: clean strip

all: zebra

hdr = config.h tools.h nutrient.h species.h point.h cell.h wall.h pipe.h \
	junction.h inlet.h junction.h network.h results.h simulation.h
src = tools.c nutrient.c species.c cell.c inlet.c junction.c network.c \
	results.c simulation.c main.c
obj = tools.o nutrient.o species.o cell.o inlet.o junction.o network.o \
	results.o simulation.o main.o
cfg = tools.h config.h Makefile
spe = species.h nutrient.h $(cfg)
pip = pipe.h cell.h wall.h point.h $(spe)
net = network.h inlet.h junction.h $(pip)
res = results.h $(net)
sim = simulation.h $(res)
libs = libxml-2.0 glib-2.0
es = locales/es/LC_MESSAGES/
locales = $(es)zebra.mo

CC = gcc
#COPT = -O3 -march=native -flto
COPT = -O3 -flto
CFLAGS = -c -g -Wall -Wextra $(COPT) `pkg-config --cflags $(libs)`
LDFLAGS = -g -flto -lm `pkg-config --libs $(libs)`

PGO = 1
ifeq ($(PGO), 1)
	objpgo = tools.pgo nutrient.pgo species.pgo cell.pgo inlet.pgo \
		 junction.pgo network.pgo results.pgo simulation.pgo main.pgo
	pgogen = -fprofile-generate
	pgouse = -fprofile-use -fprofile-correction
	toolsdep = zebrapgo
	nutrientdep = zebrapgo
	speciesdep = zebrapgo
	celldep = zebrapgo
	inletdep = zebrapgo
	junctiondep = zebrapgo
	networkdep = zebrapgo
	resultsdep = zebrapgo
	simulationdep = zebrapgo
	maindep = zebrapgo
else
	toolsdep = tools.c $(cfg)
	nutrientdep = nutrient.c nutrient.h $(cfg)
	speciesdep = species.c $(spe)
	celldep = cell.c cell.h point.h $(spe)
	inletdep = inlet.c inlet.h $(pip)
	junctiondep = junction.c junction.h $(pip)
	networkdep = network.c $(net)
	resultsdep = results.c $(res)
	simulationdep = simulation.c $(sim)
	maindep = main.c $(sim)
endif

all: zebra $(locales)

zebrapgo: $(objpgo)
	$(CC) $(objpgo) $(LDFLAGS) $(pgogen) -o zebrapgo
	./zebrapgo ../tests/test1/simulation.xml
	./zebrapgo ../tests/test1/results.bin ../tests/test1/results.xml

tools.pgo: tools.c $(cfg)
	$(CC) $(CFLAGS) $(pgogen) tools.c -o tools.pgo

nutrient.pgo: nutrient.c nutrient.h $(cfg)
	$(CC) $(CFLAGS) $(pgogen) nutrient.c -o nutrient.pgo

species.pgo: species.c $(spe)
	$(CC) $(CFLAGS) $(pgogen) species.c -o species.pgo

cell.pgo: cell.c cell.h point.h $(spe)
	$(CC) $(CFLAGS) $(pgogen) cell.c -o cell.pgo

inlet.pgo: inlet.c inlet.h $(pip)
	$(CC) $(CFLAGS) $(pgogen) inlet.c -o inlet.pgo

junction.pgo: junction.c junction.h $(pip)
	$(CC) $(CFLAGS) $(pgogen) junction.c -o junction.pgo

network.pgo: network.c $(net)
	$(CC) $(CFLAGS) $(pgogen) network.c -o network.pgo

results.pgo: results.c $(res)
	$(CC) $(CFLAGS) $(pgogen) results.c -o results.pgo

simulation.pgo: simulation.c $(sim)
	$(CC) $(CFLAGS) $(pgogen) simulation.c -o simulation.pgo

main.pgo: main.c $(sim)
	$(CC) $(CFLAGS) $(pgogen) main.c -o main.pgo

zebra: $(obj)
	$(CC) $(obj) $(LDFLAGS) -o zebra

tools.o: $(toolsdep)
	$(CC) $(CFLAGS) $(pgouse) tools.c -o tools.o

nutrient.o: $(nutrientdep)
	$(CC) $(CFLAGS) $(pgouse) nutrient.c -o nutrient.o

species.o: $(speciesdep)
	$(CC) $(CFLAGS) $(pgouse) species.c -o species.o

cell.o: $(celldep)
	$(CC) $(CFLAGS) $(pgouse) cell.c -o cell.o

inlet.o: $(inletdep)
	$(CC) $(CFLAGS) $(pgouse) inlet.c -o inlet.o

junction.o: $(junctiondep)
	$(CC) $(CFLAGS) $(pgouse) junction.c -o junction.o

network.o: $(networkdep)
	$(CC) $(CFLAGS) $(pgouse) network.c -o network.o

results.o: $(resultsdep)
	$(CC) $(CFLAGS) $(pgouse) results.c -o results.o

simulation.o: $(simulationdep)
	$(CC) $(CFLAGS) $(pgouse) simulation.c -o simulation.o

main.o: $(maindep)
	$(CC) $(CFLAGS) $(pgouse) main.c -o main.o

locales/zebra.pot: $(src) Makefile
	test -d locales || mkdir -p locales
	$(gnu)xgettext -k_ -d zebra -o locales/zebra.pot --from-code=UTF-8 \
		$(src)
	touch locales/zebra.pot

$(es)zebra.po: locales/zebra.pot
	test -d $(es) || mkdir -p $(es)
	test -f $(es)zebra.po || \
		msginit -l es -o $(es)zebra.po -i locales/zebra.pot \
			--no-translator
	msgmerge -U $(es)zebra.po locales/zebra.pot
	touch $(es)zebra.po
	vim $(es)zebra.po

$(es)zebra.mo: $(es)zebra.po
	$(gnu)msgfmt -c -v -o $(es)zebra.mo $(es)zebra.po
	touch $(es)zebra.mo

clean:
	rm zebra $(obj) $(locales) zebrapgo $(objpgo) *.gcda

strip:
	make
	strip zebra
