CXX ?= g++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -pedantic
ROOT := ..

.PHONY: all test probes clean

all: test inventory probes

descriptor_tests: test_descriptor.cpp $(ROOT)/device_capability_descriptor.cpp
	$(CXX) $(CXXFLAGS) -I$(ROOT) $^ -o $@

test: descriptor_tests
	./descriptor_tests

control_inventory_tests: test_control_inventory.cpp $(ROOT)/control_inventory.cpp $(ROOT)/device_capability_descriptor.cpp
	$(CXX) $(CXXFLAGS) -I$(ROOT) $^ -o $@

inventory: control_inventory_tests
	./control_inventory_tests

probes:
	$(CXX) $(CXXFLAGS) -I$(ROOT) -DEXPECTED_TARGET=1 target_selection_probe.cpp -o probe_default
	./probe_default
	$(CXX) $(CXXFLAGS) -I$(ROOT) -DVARIANT_125B -DEXPECTED_TARGET=1 target_selection_probe.cpp -o probe_125b
	./probe_125b
	$(CXX) $(CXXFLAGS) -I$(ROOT) -DVARIANT_1590B -DEXPECTED_TARGET=2 target_selection_probe.cpp -o probe_1590b
	./probe_1590b
	$(CXX) $(CXXFLAGS) -I$(ROOT) -DVARIANT_1590B_SMD -DEXPECTED_TARGET=3 target_selection_probe.cpp -o probe_1590b_smd
	./probe_1590b_smd
	$(CXX) $(CXXFLAGS) -I$(ROOT) -DVARIANT_TERRARIUM -DEXPECTED_TARGET=4 target_selection_probe.cpp -o probe_terrarium
	./probe_terrarium
	$(CXX) $(CXXFLAGS) -I$(ROOT) -DVARIANT_FUNBOX -DEXPECTED_TARGET=5 target_selection_probe.cpp -o probe_funbox
	./probe_funbox
	@if $(CXX) $(CXXFLAGS) -I$(ROOT) -DVARIANT_125B -DVARIANT_FUNBOX -DEXPECTED_TARGET=1 target_selection_probe.cpp -o probe_conflict >/dev/null 2>&1; then \
		echo "conflicting variants unexpectedly compiled"; exit 1; \
	else \
		echo "conflicting variants rejected: PASS"; \
	fi

clean:
	rm -f descriptor_tests descriptor_tests_ubsan control_inventory_tests control_inventory_tests_ubsan probe_default probe_125b probe_1590b probe_1590b_smd probe_terrarium probe_funbox probe_conflict
