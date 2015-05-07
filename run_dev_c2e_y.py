#!/usr/bin/python

import os
import re
import time
import sys
sys.path.append("../run_tst_c2e")
from run_tst_c2e import calc_nbest_BLEU, get_BLEU

#run_test = True
run_test = False

run = os.system

#execfile("../config.py")

max_ite = 30
opt_ite = -1
opt_bleu = 0
DEV_SRC = "../data/src.sgm"
DEV_REF = "../data/ref_lc.sgm"

def test_set_run(ite):
    cwd = os.getcwd()
    os.chdir("../run_tst_c2e")
    assert run("./run_tst_c2e.py %d" %ite) == 0
    os.chdir(cwd)

def update_BLEU(ite):
    global opt_bleu
    global opt_ite

    bleu = get_BLEU("work/o_kbest%d.bleu" %ite)
    print "iteration", ite, "bleu = ", bleu
    if bleu > opt_bleu:
        opt_bleu, opt_ite = bleu, ite
        run("cp ../config.ini ../best_config.ini") 

def update_weight(lam):
	#lines = file(lam, "rU").readlines()
	#assert len(lines) > 0
	#if "inf" in lines[2]:
	#    print "inf in lambda file"
	#    print "re-iterate..."
	#    return False
	#weights = ", ".join(lines[1].split())
	run("./genConfig.py ../weights.txt %s > tmp; mv tmp ../weights.txt" % (lam))
	#cmd = '''sed 's/\(WEIGHTS.*\[\).*\(\]\)/\\1%s\\2/g' ../config.py > tmp && mv tmp ../config.py''' %weights
	#run(cmd)
	return True

# formal run
if __name__ == "__main__":
    assert run("cp init work/init") == 0
    assert run("cp yinit work/yinit") == 0
    assert run("cp ../data/weights.txt ../weights.txt") == 0

    for ite in xrange(max_ite):
        run("cp ../config.ini work/config.ini.%d" %ite)
        #run("./chiero ../config.py dev")
        run("../cubit ../config.ini")
        run("mv kbest work/o_kbest%d" %(ite))

        calc_nbest_BLEU("work/o_kbest%d" %ite, DEV_SRC, DEV_REF)
        update_BLEU(ite)   

        if run_test:
            test_set_run(ite)

        if ite == 0:
            run("cp work/o_kbest%d work/_kbest0" %ite)
            run("./create_feature work/_kbest0 %s work/_cand0 work/_feat0 1" %(DEV_REF))
        else:
            cmd = "./merge_feature.py --o_kbest work/_kbest%d --o_cand work/_cand%d --o_feat work/_feat%d --c_kbest work/o_kbest%d  --n_kbest work/_kbest%d --n_cand work/_cand%d --n_feat work/_feat%d --ref %s" 
            cmd %= (ite - 1, ite - 1, ite - 1, ite, ite, ite, ite, DEV_REF)
            run(cmd)
        run("./format.py work/_cand%d work/_feat%d > work/yfeat%d" %(ite,ite,ite))

        if ite < max_ite - 1:
            #run("./mert work/init work/_cand%d work/_feat%d work/lambda%d" %(ite, ite, ite))
            run("./ymert work/yfeat%d work/yinit; echo 'ymert finished'; cp lambdas.out work/lambdas%d" % (ite,ite))
            #update_weight("work/lambda%d" %ite)
            update_weight("lambdas.out")

    print "MERT is over, optimal (%f, %d)" %(opt_bleu, opt_ite)
        
