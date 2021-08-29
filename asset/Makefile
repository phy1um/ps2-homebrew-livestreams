
MODELS=cube.bin MIT_teapot.bin
MODEL_CONVERT=python3 pipeline/conv.py

all: $(MODELS)

%.bin: %.obj
	$(MODEL_CONVERT) $< $@

clean:
	rm -f $(MODELS)
