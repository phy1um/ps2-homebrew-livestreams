
.PHONY: lualint
lualint:
	luac5.3 -p $(LUA_FILES)

.PHONY: lint
lint:
	cpplint --filter=$(CPPLINT_FILTERS) --counting=total --linelength=$(CPPLINT_LINE_LENGTH) --extensions=c,h --recursive src

.PHONY: format
format:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/workdir unibeautify/clang-format -i -sort-includes src/**/*.c **/*.h

.PHONY: cppcheck
cppcheck:
	$(DOCKER) run $(DOCKERFLAGS) --rm -v $(shell pwd):/src $(CPPCHECK_IMG) cppcheck -v --xml --enable=all ./src/ 2> $(CPPCHECK_REPORT)
	$(DOCKER) run $(DOCKERFLAGS) --rm -v $(shell pwd):/src $(CPPCHECK_IMG) cppcheck-htmlreport --source-dir=. --title="Engine Quality - $(VERSION)" --file=$(CPPCHECK_REPORT) --report-dir=$(CPPCHECK_OUT)

.PHONY: memcheck
memcheck:
	cd dist && valgrind --log-file="./vg-log" ./sim
