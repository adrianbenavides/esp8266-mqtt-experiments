# https://stackoverflow.com/a/6273809/1826109
%:
	@:

.PHONY: submodules_update
submodules_update:
	@git submodule foreach git pull origin master

.PHONY: copy_helpers
copy_helpers:
	@bash copy-helpers.sh