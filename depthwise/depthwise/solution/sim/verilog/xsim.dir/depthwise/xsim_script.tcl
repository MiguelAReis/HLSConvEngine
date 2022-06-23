set_param project.enableReportConfiguration 0
load_feature core
current_fileset
xsim {depthwise} -view {{depthwise_dataflow_ana.wcfg}} -tclbatch {depthwise.tcl} -protoinst {depthwise.protoinst}
