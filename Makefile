build:
	pio run -e manual
manual:
	-mkdir ./manual/pages
	.pio/build/manual/program $(SEED) > ./manual/pages/page.html