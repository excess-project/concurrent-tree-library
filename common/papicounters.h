/*
 papicounters.h

 This is part of the tree library

 Copyright 2015 Ibrahim Umar (UiT the Arctic University of Norway)

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */


#ifndef PAPICOUNTERS_H
#define PAPICOUNTERS_H

struct localcounters *prof_prepare(int);
int prof_start(struct localcounters *); 
int prof_end(struct localcounters *);
int prof_print(struct localcounters *);
int prof_print_all_threads(int, long long** );

struct localcounters{
	long long *values;
	int papiEventSet;
	long long start_time;
	long long end_time;
};

#endif
