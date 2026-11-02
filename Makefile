SHELL=/bin/bash

ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
DIR_NAME := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
MK_PATH  := $(abspath $(lastword $(MAKEFILE_LIST)))

ifneq (,$(findstring xterm,${TERM}))
	RED   := $(shell tput -Txterm setaf 1)
	GREEN := $(shell tput -Txterm setaf 2)
	RESET := $(shell tput -Txterm sgr0)
else
	RED   := ""
	GREEN := ""
	RESET := ""
endif

TARGET_COLOR := $(RED)

.PHONY: no_targets__ info deploy doc
	no_targets__:

.DEFAULT_GOAL := help

all: build-gtest build-test build-vanilla

build: build-vanilla

build-gtest:  ## build gtest [`libgtest`]
	cd vanilla-ice/test/gtest && rm -rf build && mkdir build && cd build && cmake .. && make || cd "${ROOT_DIR}"

build-test:  ## build test [`test-runner`]
	echo "TBD"

build-vanilla:  ## build demo executables for `vanilla-ice`
	cd vanilla-ice && rm -rf build && mkdir build && cd build && cmake .. && make || cd "${ROOT_DIR}"

clean:  ## remove build artifacts
	rm -rf "${ROOT_DIR}"/vanilla-ice/test/gtest/build
	rm -rf "${ROOT_DIR}"/vanilla-ice/test/build
	rm -rf "${ROOT_DIR}"/vanilla-ice/build

run-test:  ## run `test-runner`
	echo "TBD"

test: build-gtest build-test run-test

help:
	@echo "${GREEN}-----------------------------------------------------------------${RESET}"
	@echo "                          ${RED}Vanilla Ice${RESET}"
	@echo "${GREEN}-----------------------------------------------------------------${RESET}"
	@grep -E '^[a-zA-Z_0-9%-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; \
	{printf "${TARGET_COLOR}%-20s${RESET} %s\n", $$1, $$2}'
