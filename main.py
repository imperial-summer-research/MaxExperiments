
import os
import sys
import argparse
import shlex
from datetime import datetime
from subprocess import Popen, PIPE
from multiprocessing import Process
from time import sleep
from termcolor import colored

# configuration of current workspace
work_space_dir = '/home/vzhao'
work_space_projs = [
    'SummationTree'
]

class MaxCompilerDesign:

	# [root_path]: should be an absolute path
	# [proj_name]: same as the proj_name in Makefile. It should also be the dir base
	# [**kwargs]:  configurations of your design, such as number of pipes, etc.
	# All these params will be used to generate project full name
	def __init__(self, root_path, proj_name, **proj_conf):
		self.root_path = root_path
		self.proj_name = proj_name
		self.proj_conf = proj_conf
		self.proj_path = '%s/%s' % (self.root_path, self.proj_name)
		self.build_abs_path = '%s/build' % (self.proj_path)
		# const currently
		self.remote_server = 'maxnode2'

	def log(self, s, cmd):
		timestamp = datetime.now().isoformat()
		with open('%s/logs/%s_%s.log' % (self.proj_path, cmd, timestamp), 'w') as f:
			print >>f, s

	def build_make_str(self, cmd):
		return 'make ' + cmd + ' ' + ' '.join(['%s=%03d' % (k.upper(),int(v)) for k,v in self.proj_conf.iteritems()])

	def signal(self, s):
		print '[%s:%03d] %s' % (os.getppid(), self.proj_conf['numPipes'], s)

	def popen(self, cmd, append_list=[]):
		cmd_list = shlex.split(cmd) + append_list
		self.signal('split cmd: {}'.format(cmd_list))
		p = Popen(cmd_list, stdout=PIPE, stderr=PIPE)
		# interactive communicate 
		while True:
			nextline = p.stdout.readline()
			if not nextline:
				sleep(1)
			else:
				self.signal(nextline.strip())
			if p.poll() is not None:
				self.signal("received term signal")
				nextlines = p.stdout.readlines()
				if nextlines:
					for nextline in nextlines:
						self.signal(nextline.strip())
				break

		# logging
		stdout, stderr = p.communicate()
		self.log(stdout, cmd_list[0])

	def make(self, cmd):
		os.chdir(self.build_abs_path)
		# running and logging
		self.signal(self.build_make_str(cmd))
		self.popen(self.build_make_str(cmd))
		self.signal("Finished building ...")
		os.chdir(self.root_path)

	def sync(self):
		self.signal('Start to sync to remote [%s]' % self.remote_server)
		remote_proj_path = '{}:{}'.format(self.remote_server, self.proj_path)
		rsync_cmd = 'rsync -avz {} {}'.format(self.proj_path, remote_proj_path)
		self.signal(rsync_cmd)
		self.popen(rsync_cmd)
	
	def remote_run(self, cmd):
		run_cmd = "ssh {}".format(self.remote_server)
		app_cmd = "cd {} && {} 2>&1".format(self.build_abs_path, cmd)
		append_list = [app_cmd]
		self.signal(run_cmd)
		self.popen(run_cmd, append_list)

design_name = 'StandardCSRpSpMVOptArea'

def info(title):
    if hasattr(os, 'getppid'):  # only available on Unix
        print 'parent process:', os.getppid()
    print 'process id:', os.getpid()

def make_design(num_pipes, num_max_columns=8192, stream_frequency=200):
	return MaxCompilerDesign('/home/vz', design_name, 
			numPipes=num_pipes, 
			numMaxColumns=num_max_columns, 
			streamFrequency=stream_frequency)

class MaxCompilerWorkspace:
    def __init__(self, work_space_path, work_space_projs=[]):
        path_split = work_space_path.split(os.sep)
        root, name = path_split[0:-1], path_split[-1]
        self.name = name
        self.root = os.sep.join(root)
        self.path = "{}/{}".format(self.root, self.name)
        self.proj_list = work_space_projs

        print "MaxCompiler Work Space:", colored(self.name, 'green')
        print "Path:\t {}".format(self.path)
        print "List of projects:"
        for idx, proj in enumerate(self.proj_list):
            print "{:3d}:\t{}".format(idx, proj)


def run_design_build(num_pipes, num_max_columns=8192, stream_frequency=200):
	info('design build')

	design = make_design(num_pipes, num_max_columns, stream_frequency)
	design.make('build')
	#design.sync()

def run_design(num_pipes, num_max_columns=8192, stream_frequency=200):
	info('design run')
	design = make_design(num_pipes, num_max_columns, stream_frequency)
	design.remote_run(design.build_make_str('run'))

#def 

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Run Max Compiler Design')
    parser.add_argument('-w', '--work_space_path', default='/home/vzhao/MaxExperiment')
    parser.add_argument('-p', '--proj_name_list', nargs='*')
    args = parser.parse_args(sys.argv[1:])

    max_work_space = MaxCompilerWorkspace(args.work_space_path, args.proj_name_list)
#	max_pipes = int(args.max_pipes)
#	design_name = args.design_name
#	print '[main] Number of pipes to build:', max_pipes
#
#	info('main')
#
#	main_design = make_design(1)
#
#	ps = []
#	for i in range(10):
#		p = Process(target=run_design_build, args=(2**i,))
#		p.start()
#		ps.append(p)
#
#	for p in ps:
#		p.join()
#
#	main_design.sync()
#
#	print "[main] finished build and sync tasks"
#
#	# Here to run these commands sequentially
#	for i in range(max_pipes):
#		run_design(2**i)
