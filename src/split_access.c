/**   LICENSE
* Copyright (c) 2014-2018 Genome Research Ltd.
*
* Author: Cancer Genome Project cgpit@sanger.ac.uk
*
* This file is part of CaVEMan.
*
* CaVEMan is free software: you can redistribute it and/or modify it under
* the terms of the GNU Affero General Public License as published by the Free
* Software Foundation; either version 3 of the License, or (at your option) any
* later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
* details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*
*    1. The usage of a range of years within a copyright statement contained within
*    this distribution should be interpreted as being equivalent to a list of years
*    including the first and last year specified and all consecutive years between
*    them. For example, a copyright statement that reads ‘Copyright (c) 2005, 2007-
*    2009, 2011-2012’ should be interpreted as being identical to a statement that
*    reads ‘Copyright (c) 2005, 2007, 2008, 2009, 2011, 2012’ and a copyright
*    statement that reads ‘Copyright (c) 2005-2012’ should be interpreted as being
*    identical to a statement that reads ‘Copyright (c) 2005, 2006, 2007, 2008,
*    2009, 2010, 2011, 2012’."
*
*/

#include <split_access.h>
#include <dbg.h>
#include <List.h>
#include <ignore_reg_access.h>
#include <assert.h>

int split_access_print_section(FILE *output, char *chr, int start_one_based, int stop){
	assert(output !=NULL);
	return (fprintf(output,"%s\t%d\t%d\n",chr,start_one_based-1,stop));
}

void split_access_get_section_from_index(char *file_loc, char *chr, int *start_zero_based, int *stop, int index){
	assert(index > 0);
	assert(file_loc != NULL);
	assert(chr != NULL);
	FILE *file;
	file = fopen(file_loc,"r");
	check(file != NULL,"Error opening split list file.");
	char line[250];
	int i=1;
	while ( fgets(line,sizeof(line),file) != NULL ){
		if(index == i){
			int chk = sscanf(line,"%s\t%d\t%d",chr,start_zero_based,stop);
			check(chk==3,"Error parsing split file line number %d: %s.",i,line);
			check(fclose(file)==0,"Error closing split file.");
			return;
		}
		i++;
	}

error:
	if(file) fclose(file);
	return;
}

List *split_access_get_all_split_sections(char *file_loc){
	assert(file_loc != NULL);
	FILE *file;
	char *chr = NULL;
	seq_region_t *reg = NULL;
	file = fopen(file_loc,"r");
	check(file != NULL,"Error opening split list file.");
	char line[250];
	int i=0;
	List *li = List_create();

	while ( fgets(line,sizeof(line),file) != NULL ){
		i++;
		chr = malloc(sizeof(char) * 250);
		check_mem(chr);
		int start_zero_based = 0;
		int stop = 0;
		int chk = sscanf(line,"%s\t%d\t%d",chr,&start_zero_based,&stop);
		check(chk==3,"Error parsing split file line number %d: %s.",i,line);
		reg = malloc(sizeof(struct seq_region_t));
		check_mem(reg);
		reg->beg = start_zero_based+1;
		reg->end = stop;
		reg->chr_name = chr;
		List_push(li,reg);
	}
	return li;
error:
	if(reg){
		if(reg->chr_name) free(reg->chr_name);
		free(reg);
	}
	if(chr) free(chr);
	return NULL;
}
