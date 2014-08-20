# The `interface implementation' in form of a library:
lib_LIBRARIES = libcpp11.a
libcpp11_a_SOURCES = src/cpp11/cpp11.h src/cpp11/cpp11.cc \
		     src/cpp11/factorial.h src/cpp11/factorial.cc
libcpp11_HEADERS=src/cpp11/cpp11.h
libcpp11dir=$(includedir)/cpp11

# A dummy only to check if linkable without extra deps:
noinst_PROGRAMS=dummy
dummy_SOURCES=dummy.cc
dummy_DEPENDENCIES=libcpp11.a
dummy_LDADD=libcpp11.a

# CppUnit testrunner with linked-in test cases
TESTS=testrunner
check_PROGRAMS=testrunner
testrunner_SOURCES=test/testrunner.cc test/cpp11test.cc
testrunner_DEPENDENCIES=libcpp11.a
testrunner_LDADD=libcpp11.a $(CPPUNIT_LIBS)

# Administrativa.
AUTOMAKE_OPTIONS = subdir-objects
dist_noinst_SCRIPTS = autogen.sh


#test -e $$stamp || touch $$stamp

autotest:
	@echo "Will run \`make check' after each change, stop with INT." ; \
	while true ; do \
	  if ! make --question -s -j4 testrunner ; then \
	    echo "Rechecking..." ; \
	      make -s check || { \
	        echo "failed to run `date`" >> testrunner.log ; \
	      } ; \
	  fi ; \
	  sleep 1 ; \
	done

