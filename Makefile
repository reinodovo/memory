pdf:
	pio run -t exec -e native -a $(SEED)
	defusal manual.md -n Memory -o manual.pdf -v $(SEED)