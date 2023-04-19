SUBDIRS = cobol85

TESTS = syntax \
	run \
	data-rep 
	
syntax_data = \
	syntax.at \
	syntax.src/copy.at \
	syntax.src/definition.at \
	syntax.src/expression.at \
	syntax.src/multiply.at \
	syntax.src/redefines.at \
	syntax.src/value.at \
	syntax.src/move.at \
	syntax.src/set.at \
	syntax.src/occurs.at \
	syntax.src/subscripts.at

run_data = \
	run.at \
	run.src/fundamental.at \
	run.src/accept.at \
	run.src/initialize.at \
	run.src/numeric.at \
	run.src/misc.at \
	run.src/ref-mod.at \
	run.src/subscripts.at \
	run.src/extensions.at \
	run.src/return-code.at \
	run.src/functions.at 

data_rep_data = \
	data-rep.at \
	data-rep.src/binary.at \
	data-rep.src/display.at \
	data-rep.src/packed.at \
	data-rep.src/pointer.at \
	data-rep.src/compute.at \
	data-rep.src/numeric-dump.cob \
	data-rep.src/numeric-ns-dump.cob \
	data-rep.src/numeric-display.cob \
	data-rep.src/TSTMFC5.cbl \
	data-rep.src/isam.at \
	data-rep.src/isam/test1.cbl \
	data-rep.src/isam/test2.cbl \
	data-rep.src/isam/stockcli.lseq \
	data-rep.src/isam/stockcli.cms \
	data-rep.src/isam/RELFIX.cob \
	data-rep.src/isam/SEQLINE.cob \
	data-rep.src/isam/SEQVAR.cob \
	data-rep.src/isam/VSAMFIX.cob \
	data-rep.src/isam/VSAMVAR.cob 


	
EXTRA_DIST = package.m4 $(TESTS) all-test.sh all-test-O.sh \
	$(syntax_data) \
	$(run_data) \
	$(data_rep_data) 

DISTCLEANFILES = atconfig

all: $(TESTS)

clean-local:
	rm -rf *.dir *.log

package.m4: $(top_srcdir)/configure.ac
	@echo "creating package.m4"
	@{								   \
	  echo '# Signature of the current package.';			   \
	  echo 'm4_define([AT_PACKAGE_NAME],	  [@PACKAGE_NAME@])';	   \
	  echo 'm4_define([AT_PACKAGE_TARNAME],	  [@PACKAGE_TARNAME@])';   \
	  echo 'm4_define([AT_PACKAGE_VERSION],	  [@PACKAGE_VERSION@])';   \
	  echo 'm4_define([AT_PACKAGE_STRING],	  [@PACKAGE_STRING@])';    \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	} > $(srcdir)/package.m4

%: %.at package.m4 atlocal.in
	autom4te --language=autotest -I . -I $@.src -o $@ $<
	chmod +x $@

syntax: $(syntax_data)
run: $(run_data)
data-rep: $(data_rep_data)


