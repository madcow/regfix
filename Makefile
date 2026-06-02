PLATFORM  := x86
CONFIG    := release
SOLUTION  := tools\regfix.sln
MSBUILD   := tools\msbuild.bat

.PHONY: build
build:
	@echo [CC] Building with config '$(CONFIG)'...
	@$(MSBUILD) "$(SOLUTION)" /nologo /verbosity:quiet /t:build \
	/p:platform=$(PLATFORM) /p:configuration=$(CONFIG)

.PHONY: clean
clean:
	@echo [RM] Cleaning build files...
	@$(MSBUILD) "$(SOLUTION)" /nologo /verbosity:quiet \
		/t:clean