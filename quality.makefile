
.PHONY: lualint
lualint:
	luac5.3 -p $(LUA_FILES)

.PHONY: lint
lint:
	cpplint --filter=$(CPPLINT_FILTERS) --counting=total --linelength=$(CPPLINT_LINE_LENGTH) --extensions=c,h --recursive .

.PHONY: format
format:
	$(DOCKER) run $(DOCKERFLAGS) -v $(shell pwd):/workdir unibeautify/clang-format -i -sort-includes **/*.c **/*.h

.PHONY: cppcheck
cppcheck:
	$(DOCKER) run --rm -v $(shell pwd):/src $(CPPCHECK_IMG) cppcheck -v --xml --enable=all . 2> $(CPPCHECK_REPORT)
	$(DOCKER) run --rm -v $(shell pwd):/src $(CPPCHECK_IMG) cppcheck-htmlreport --source-dir=. --title="Engine Quality - $(VERSION)" --file=$(CPPCHECK_REPORT) --report-dir=$(CPPCHECK_OUT)
