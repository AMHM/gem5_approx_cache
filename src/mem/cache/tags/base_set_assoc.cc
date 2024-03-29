/*
 * Copyright (c) 2012-2014 ARM Limited
 * All rights reserved.
 *
 * The license below extends only to copyright in the software and shall
 * not be construed as granting a license to any other intellectual
 * property including but not limited to intellectual property relating
 * to a hardware implementation of the functionality of the software
 * licensed hereunder.  You may use the software subject to the license
 * terms below provided that you ensure that this notice is replicated
 * unmodified and in its entirety in all distributions of the software,
 * modified or unmodified, in source code or in binary form.
 *
 * Copyright (c) 2003-2005,2014 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Erik Hallnor
 */

/**
 * @file
 * Definitions of a base set associative tag store.
 */

#include "mem/cache/tags/base_set_assoc.hh"

#include <string>

#include "base/intmath.hh"
#include "sim/core.hh"

//AMHM Start
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "base/callback.hh"
//AMHM End

using namespace std;

BaseSetAssoc::BaseSetAssoc(const Params *p)
    :BaseTags(p), assoc(p->assoc), allocAssoc(p->assoc),
     numSets(p->size / (p->block_size * p->assoc)),
     sequentialAccess(p->sequential_access),
	 outdir(p->outdir)
{
    // Check parameters
    if (blkSize < 4 || !isPowerOf2(blkSize)) {
        fatal("Block size must be at least 4 and a power of 2");
    }
    if (numSets <= 0 || !isPowerOf2(numSets)) {
        fatal("# of sets must be non-zero and a power of 2");
    }
    if (assoc <= 0) {
        fatal("associativity must be greater than zero");
    }

    //AMHM Start
    FILE * STTRAMCellConfig;
    STTRAMCellConfig = fopen ("..//gem5_approx_cache//configs//FI_Configurations//STT-RAMCacheConfig_Default.cfg", "r");
	FILE * STTRAMFIConfig;
    STTRAMFIConfig = fopen ("..//gem5_approx_cache//configs//FI_Configurations//STTRAMFIConfigL1L2.cfg", "r");
    char stringTemp0[500];
    char stringTemp1[500];
    double value1 = 0;
    double value2 = 0;

    cacheWay = new way[assoc];

    Callback* cb = new MakeCallback<BaseSetAssoc,
                &BaseSetAssoc::wayAccessAnalysisOutput>(this);
    registerExitCallback(cb);

    if (STTRAMCellConfig==NULL) {
      printf("The STT-RAM cache config file could not be opened!\n");
    }
    else {
        if (fgets(stringTemp0, sizeof(stringTemp0), STTRAMCellConfig) != NULL) {
            while(STTRAMCellConfig) {
                if(fscanf(STTRAMCellConfig, "%s %le %le %s", stringTemp0, &value1, &value2, stringTemp1) == 4) {
                    if(!strcmp(stringTemp0, "L1IreadErrorRateLevel0")) {
                        if((name() == "system.cpu.icache.tags")||
                            (name() == "system.cpu0.icache.tags")||
						    (name() == "system.cpu1.icache.tags")||
							(name() == "system.cpu2.icache.tags")||
							(name() == "system.cpu3.icache.tags")){
                            readErrorRateLevel0 = value1;
                            readErrorRateLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DreadErrorRateLevel0")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            readErrorRateLevel0 = value1;
                            readErrorRateLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2readErrorRateLevel0")) {
                        if(name() == "system.l2.tags"){
                            readErrorRateLevel0 = value1;
                            readErrorRateLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1IwriteErrorRateLevel0")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            writeErrorRateLevel0 = value1;
                            writeErrorRateLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DwriteErrorRateLevel0")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            writeErrorRateLevel0 = value1;
                            writeErrorRateLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2writeErrorRateLevel0")) {
                        if(name() == "system.l2.tags"){
                            writeErrorRateLevel0 = value1;
                            writeErrorRateLevel0Var = value2;
                        }
                    }

                    else if(!strcmp(stringTemp0, "L1IreadErrorRateLevel1")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            readErrorRateLevel1 = value1;
                            readErrorRateLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DreadErrorRateLevel1")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            readErrorRateLevel1 = value1;
                            readErrorRateLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2readErrorRateLevel1")) {
                        if(name() == "system.l2.tags"){
                            readErrorRateLevel1 = value1;
                            readErrorRateLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1IwriteErrorRateLevel1")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            writeErrorRateLevel1 = value1;
                            writeErrorRateLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DwriteErrorRateLevel1")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            writeErrorRateLevel1 = value1;
                            writeErrorRateLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2writeErrorRateLevel1")) {
                        if(name() == "system.l2.tags"){
                            writeErrorRateLevel1 = value1;
                            writeErrorRateLevel1Var = value2;
                        }
                    }

                    else if(!strcmp(stringTemp0, "L1IreadErrorRateLevel2")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            readErrorRateLevel2 = value1;
                            readErrorRateLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DreadErrorRateLevel2")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            readErrorRateLevel2 = value1;
                            readErrorRateLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2readErrorRateLevel2")) {
                        if(name() == "system.l2.tags"){
                            readErrorRateLevel2 = value1;
                            readErrorRateLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1IwriteErrorRateLevel2")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            writeErrorRateLevel2 = value1;
                            writeErrorRateLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DwriteErrorRateLevel2")) {
                        if((name() == "system.cpu.dcache.tags")||(name() == "system.cpu0.dcache.tags")||(name() == "system.cpu1.dcache.tags")||(name() == "system.cpu2.dcache.tags")||(name() == "system.cpu3.dcache.tags")){
                            writeErrorRateLevel2 = value1;
                            writeErrorRateLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2writeErrorRateLevel2")) {
                        if(name() == "system.l2.tags"){
                            writeErrorRateLevel2 = value1;
                            writeErrorRateLevel2Var = value2;
                        }
                    }

                    else if(!strcmp(stringTemp0, "L1IreadErrorRateLevel3")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            readErrorRateLevel3 = value1;
                            readErrorRateLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DreadErrorRateLevel3")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            readErrorRateLevel3 = value1;
                            readErrorRateLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2readErrorRateLevel3")) {
                        if(name() == "system.l2.tags"){
                            readErrorRateLevel3 = value1;
                            readErrorRateLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1IwriteErrorRateLevel3")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            writeErrorRateLevel3 = value1;
                            writeErrorRateLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L1DwriteErrorRateLevel3")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            writeErrorRateLevel3 = value1;
                            writeErrorRateLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "L2writeErrorRateLevel3")) {
                        if(name() == "system.l2.tags"){
                            writeErrorRateLevel3 = value1;
                            writeErrorRateLevel3Var = value2;
                        }
                    }

                    else if(!strcmp(stringTemp0, "dynamicL1IReadEnergyConsumptionLevel0")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            dynamicReadEnergyConsumptionLevel0 = value1;
                            dynamicReadEnergyConsumptionLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DReadEnergyConsumptionLevel0")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicReadEnergyConsumptionLevel0 = value1;
                            dynamicReadEnergyConsumptionLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2ReadEnergyConsumptionLevel0")) {
                        if(name() == "system.l2.tags"){
                            dynamicReadEnergyConsumptionLevel0 = value1;
                            dynamicReadEnergyConsumptionLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1IWriteEnergyConsumptionLevel0")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            dynamicWriteEnergyConsumptionLevel0 = value1;
                            dynamicWriteEnergyConsumptionLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DWriteEnergyConsumptionLevel0")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicWriteEnergyConsumptionLevel0 = value1;
                            dynamicWriteEnergyConsumptionLevel0Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2WriteEnergyConsumptionLevel0")) {
                        if(name() == "system.l2.tags"){
                            dynamicWriteEnergyConsumptionLevel0 = value1;
                            dynamicWriteEnergyConsumptionLevel0Var = value2;
                        }
                    }

                    else if(!strcmp(stringTemp0, "dynamicL1IReadEnergyConsumptionLevel1")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            dynamicReadEnergyConsumptionLevel1 = value1;
                            dynamicReadEnergyConsumptionLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DReadEnergyConsumptionLevel1")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicReadEnergyConsumptionLevel1 = value1;
                            dynamicReadEnergyConsumptionLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2ReadEnergyConsumptionLevel1")) {
                        if(name() == "system.l2.tags"){
                            dynamicReadEnergyConsumptionLevel1 = value1;
                            dynamicReadEnergyConsumptionLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1IWriteEnergyConsumptionLevel1")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            dynamicWriteEnergyConsumptionLevel1 = value1;
                            dynamicWriteEnergyConsumptionLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DWriteEnergyConsumptionLevel1")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicWriteEnergyConsumptionLevel1 = value1;
                            dynamicWriteEnergyConsumptionLevel1Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2WriteEnergyConsumptionLevel1")) {
                        if(name() == "system.l2.tags"){
                            dynamicWriteEnergyConsumptionLevel1 = value1;
                            dynamicWriteEnergyConsumptionLevel1Var = value2;
                        }
                    }

                    else if(!strcmp(stringTemp0, "dynamicL1IReadEnergyConsumptionLevel2")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            dynamicReadEnergyConsumptionLevel2 = value1;
                            dynamicReadEnergyConsumptionLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DReadEnergyConsumptionLevel2")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicReadEnergyConsumptionLevel2 = value1;
                            dynamicReadEnergyConsumptionLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2ReadEnergyConsumptionLevel2")) {
                        if(name() == "system.l2.tags"){
                            dynamicReadEnergyConsumptionLevel2 = value1;
                            dynamicReadEnergyConsumptionLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1IWriteEnergyConsumptionLevel2")) {
                        if((name() == "system.cpu.icache.tags")||
                          (name() == "system.cpu0.icache.tags")||
						  (name() == "system.cpu1.icache.tags")||
						  (name() == "system.cpu2.icache.tags")||
						  (name() == "system.cpu3.icache.tags")){
                            dynamicWriteEnergyConsumptionLevel2 = value1;
                            dynamicWriteEnergyConsumptionLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DWriteEnergyConsumptionLevel2")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicWriteEnergyConsumptionLevel2 = value1;
                            dynamicWriteEnergyConsumptionLevel2Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2WriteEnergyConsumptionLevel2")) {
                        if(name() == "system.l2.tags"){
                            dynamicWriteEnergyConsumptionLevel2 = value1;
                            dynamicWriteEnergyConsumptionLevel2Var = value2;
                    }

                    } else if(!strcmp(stringTemp0, "dynamicL1IReadEnergyConsumptionLevel3")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            dynamicReadEnergyConsumptionLevel3 = value1;
                            dynamicReadEnergyConsumptionLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DReadEnergyConsumptionLevel3")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicReadEnergyConsumptionLevel3 = value1;
                            dynamicReadEnergyConsumptionLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2ReadEnergyConsumptionLevel3")) {
                        if(name() == "system.l2.tags"){
                            dynamicReadEnergyConsumptionLevel3 = value1;
                            dynamicReadEnergyConsumptionLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1IWriteEnergyConsumptionLevel3")) {
                        if((name() == "system.cpu.icache.tags")||
                           (name() == "system.cpu0.icache.tags")||
						   (name() == "system.cpu1.icache.tags")||
						   (name() == "system.cpu2.icache.tags")||
						   (name() == "system.cpu3.icache.tags")){
                            dynamicWriteEnergyConsumptionLevel3 = value1;
                            dynamicWriteEnergyConsumptionLevel3Var = value2;
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL1DWriteEnergyConsumptionLevel3")) {
                        if((name() == "system.cpu.dcache.tags")||
                           (name() == "system.cpu0.dcache.tags")||
						   (name() == "system.cpu1.dcache.tags")||
						   (name() == "system.cpu2.dcache.tags")||
						   (name() == "system.cpu3.dcache.tags")){
                            dynamicWriteEnergyConsumptionLevel3 = value1;
                            dynamicWriteEnergyConsumptionLevel3Var = value2;
                            printf("ISLPED\n");
                        }
                    } else if(!strcmp(stringTemp0, "dynamicL2WriteEnergyConsumptionLevel3")) {
                        if(name() == "system.l2.tags"){
                            dynamicWriteEnergyConsumptionLevel3 = value1;
                            dynamicWriteEnergyConsumptionLevel3Var = value2;
                        }
                    }
					else {
							printf("Errors in reading STT-RAM cache config file!(1)\n");
							break;
						}
				} else
					break;
            }
        }else
        	printf("Errors in reading STT-RAM cache config file!(2)\n");
        fclose(STTRAMCellConfig);
    }
    if (STTRAMFIConfig==NULL) {
          printf("The STT-RAM fault injection config file could not be opened!\n");
    }
    else {
    	if (fgets(stringTemp0, sizeof(stringTemp0), STTRAMFIConfig) != NULL) {
    				int temp_flag;
    	            while(STTRAMFIConfig) {
    	            	if(fscanf(STTRAMFIConfig, "%s %d %s", stringTemp0, &temp_flag, stringTemp1) == 3) {
    	            		if(!strcmp(stringTemp0, "L1IerrorEnable")) {
								if((name() == "system.cpu.icache.tags")||(name() == "system.cpu0.icache.tags")||(name() == "system.cpu1.icache.tags")||(name() == "system.cpu2.icache.tags")||(name() == "system.cpu3.icache.tags"))
									faultInjection = temp_flag;
							}
    	            		else if(!strcmp(stringTemp0, "L1DerrorEnable")) {
								if((name() == "system.cpu.dcache.tags")||(name() == "system.cpu0.dcache.tags")||(name() == "system.cpu1.dcache.tags")||(name() == "system.cpu2.dcache.tags")||(name() == "system.cpu3.dcache.tags"))
									faultInjection = temp_flag;
							}
    	            		else if(!strcmp(stringTemp0, "L2errorEnable")) {
								if(name() == "system.l2.tags")
									faultInjection = temp_flag;
							}
    	            		else {
								printf("Errors in reading STT-RAM fault injection config file!(1)\n");
								break;
							}
    	            	}
    	            	else
							break;
    	            }
    	}
		else
			printf("Errors in reading STT-RAM fault injection config file!(2)\n");
    	fclose(STTRAMFIConfig);
    }
    //AMHM End

    blkMask = blkSize - 1;
    setShift = floorLog2(blkSize);
    setMask = numSets - 1;
    tagShift = setShift + floorLog2(numSets);
    /** @todo Make warmup percentage a parameter. */
    warmupBound = numSets * assoc;

    sets = new SetType[numSets];
    blks = new BlkType[numSets * assoc];
    // allocate data storage in one big chunk
    numBlocks = numSets * assoc;
    dataBlks = new uint8_t[numBlocks * blkSize];

    //AMHM Start
    for (unsigned i = 0; i < assoc; ++i) {
    	cacheWay[i].L0Accesses = 0;
    	cacheWay[i].L1Accesses = 0;
    	cacheWay[i].L2Accesses = 0;
    	cacheWay[i].L3Accesses = 0;
    	cacheWay[i].totalAccess = 0;
    }
    //AMHM End
    unsigned blkIndex = 0;       // index into blks array
    for (unsigned i = 0; i < numSets; ++i) {
        sets[i].assoc = assoc;

        //AMHM Start
        sets[i].L0Accesses = 0;
        sets[i].L1Accesses = 0;
        sets[i].L2Accesses = 0;
        sets[i].L3Accesses = 0;
        sets[i].totalAccesses = 0;
        //AMHM End

        sets[i].blks = new BlkType*[assoc];

        // link in the data blocks
        for (unsigned j = 0; j < assoc; ++j) {
            // locate next cache block
            BlkType *blk = &blks[blkIndex];
            blk->data = &dataBlks[blkSize*blkIndex];
            ++blkIndex;

            // invalidate new cache block
            blk->invalidate();

            //EGH Fix Me : do we need to initialize blk?

            // Setting the tag to j is just to prevent long chains in the hash
            // table; won't matter because the block is invalid
            blk->tag = j;
            blk->whenReady = 0;
            blk->isTouched = false;
            blk->size = blkSize;
            sets[i].blks[j]=blk;
            blk->set = i;
            blk->way = j;

            //AMHM Start
            blk->randSeed = i * j;
            srand(blk->randSeed); //each block has its own seed number!
            for (int t = 0; t < (blkSize * 8); ++t)
            {              
              //readErrorRateLevel0
              if ((rand()%100) >= 50) {
                blk->readErrorRateLevel0[t] = readErrorRateLevel0 + (((double) (rand() % 100) / 100) * readErrorRateLevel0Var) * readErrorRateLevel0;
                
              } else {
                blk->readErrorRateLevel0[t] = readErrorRateLevel0 + (((double) (rand() % 100) / 100) * readErrorRateLevel0Var) * readErrorRateLevel0;
              }

              //readErrorRateLevel1
              if ((rand()%100) >= 50) {
                blk->readErrorRateLevel1[t] = readErrorRateLevel1 + (((double) (rand() % 100) / 100) * readErrorRateLevel1Var) * readErrorRateLevel1;
                
              } else {
                blk->readErrorRateLevel1[t] = readErrorRateLevel1 + (((double) (rand() % 100) / 100) * readErrorRateLevel1Var) * readErrorRateLevel1;
              }

              //readErrorRateLevel2
              if ((rand()%100) >= 50) {
                blk->readErrorRateLevel2[t] = readErrorRateLevel2 + (((double) (rand() % 100) / 100) * readErrorRateLevel2Var) * readErrorRateLevel2;
                
              } else {
                blk->readErrorRateLevel2[t] = readErrorRateLevel2 + (((double) (rand() % 100) / 100) * readErrorRateLevel2Var) * readErrorRateLevel2;
              }

              //readErrorRateLevel3
              if ((rand()%100) >= 50) {
                blk->readErrorRateLevel3[t] = readErrorRateLevel3 + (((double) (rand() % 100) / 100) * readErrorRateLevel3Var) * readErrorRateLevel3;
                
              } else {
                blk->readErrorRateLevel3[t] = readErrorRateLevel3 + (((double) (rand() % 100) / 100) * readErrorRateLevel3Var) * readErrorRateLevel3;
              }

              //writeErrorRateLevel0
              if ((rand()%100) >= 50) {
                blk->writeErrorRateLevel0[t] = writeErrorRateLevel0 + (((double) (rand() % 100) / 100) * writeErrorRateLevel0Var) * writeErrorRateLevel0;
                
              } else {
                blk->writeErrorRateLevel0[t] = writeErrorRateLevel0 + (((double) (rand() % 100) / 100) * writeErrorRateLevel0Var) * writeErrorRateLevel0;
              }

              //writeErrorRateLevel1
              if ((rand()%100) >= 50) {
                blk->writeErrorRateLevel1[t] = writeErrorRateLevel1 + (((double) (rand() % 100) / 100) * writeErrorRateLevel1Var) * writeErrorRateLevel1;
                
              } else {
                blk->writeErrorRateLevel1[t] = writeErrorRateLevel1 + (((double) (rand() % 100) / 100) * writeErrorRateLevel1Var) * writeErrorRateLevel1;
              }

              //writeErrorRateLevel2
              if ((rand()%100) >= 50) {
                blk->writeErrorRateLevel2[t] = writeErrorRateLevel2 + (((double) (rand() % 100) / 100) * writeErrorRateLevel2Var) * writeErrorRateLevel2;
                
              } else {
                blk->writeErrorRateLevel2[t] = writeErrorRateLevel2 + (((double) (rand() % 100) / 100) * writeErrorRateLevel2Var) * writeErrorRateLevel2;
              }

              //writeErrorRateLevel3
              if ((rand()%100) >= 50) {
                blk->writeErrorRateLevel3[t] = writeErrorRateLevel3 + (((double) (rand() % 100) / 100) * writeErrorRateLevel3Var) * writeErrorRateLevel3;
                
              } else {
                blk->writeErrorRateLevel3[t] = writeErrorRateLevel3 + (((double) (rand() % 100) / 100) * writeErrorRateLevel3Var) * writeErrorRateLevel3;
              }

              //dynamicReadEnergyConsumptionLevel0
              if ((rand()%100) >= 50) {
                blk->dynamicReadEnergyConsumptionLevel0[t] = dynamicReadEnergyConsumptionLevel0 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel0Var) * dynamicReadEnergyConsumptionLevel0;
                
              } else {
                blk->dynamicReadEnergyConsumptionLevel0[t] = dynamicReadEnergyConsumptionLevel0 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel0Var) * dynamicReadEnergyConsumptionLevel0;
              }

              //dynamicReadEnergyConsumptionLevel1
              if ((rand()%100) >= 50) {
                blk->dynamicReadEnergyConsumptionLevel1[t] = dynamicReadEnergyConsumptionLevel1 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel1Var) * dynamicReadEnergyConsumptionLevel1;
                
              } else {
                blk->dynamicReadEnergyConsumptionLevel1[t] = dynamicReadEnergyConsumptionLevel1 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel1Var) * dynamicReadEnergyConsumptionLevel1;
              }

              //dynamicReadEnergyConsumptionLevel2
              if ((rand()%100) >= 50) {
                blk->dynamicReadEnergyConsumptionLevel2[t] = dynamicReadEnergyConsumptionLevel2 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel2Var) * dynamicReadEnergyConsumptionLevel2;
                
              } else {
                blk->dynamicReadEnergyConsumptionLevel2[t] = dynamicReadEnergyConsumptionLevel2 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel2Var) * dynamicReadEnergyConsumptionLevel2;
              }

              //dynamicReadEnergyConsumptionLevel3
              if ((rand()%100) >= 50) {
                blk->dynamicReadEnergyConsumptionLevel3[t] = dynamicReadEnergyConsumptionLevel3 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel3Var) * dynamicReadEnergyConsumptionLevel3;
                
              } else {
                blk->dynamicReadEnergyConsumptionLevel3[t] = dynamicReadEnergyConsumptionLevel3 + (((double) (rand() % 100) / 100) * dynamicReadEnergyConsumptionLevel3Var) * dynamicReadEnergyConsumptionLevel3;
              }

              //dynamicWriteEnergyConsumptionLevel0
              if ((rand()%100) >= 50) {
                blk->dynamicWriteEnergyConsumptionLevel0[t] = dynamicWriteEnergyConsumptionLevel0 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel0Var) * dynamicWriteEnergyConsumptionLevel0;
                
              } else {
                blk->dynamicWriteEnergyConsumptionLevel0[t] = dynamicWriteEnergyConsumptionLevel0 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel0Var) * dynamicWriteEnergyConsumptionLevel0;
              }

              //dynamicWriteEnergyConsumptionLevel1
              if ((rand()%100) >= 50) {
                blk->dynamicWriteEnergyConsumptionLevel1[t] = dynamicWriteEnergyConsumptionLevel1 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel1Var) * dynamicWriteEnergyConsumptionLevel1;
                
              } else {
                blk->dynamicWriteEnergyConsumptionLevel1[t] = dynamicWriteEnergyConsumptionLevel1 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel1Var) * dynamicWriteEnergyConsumptionLevel1;
              }

              //dynamicWriteEnergyConsumptionLevel2
              if ((rand()%100) >= 50) {
                blk->dynamicWriteEnergyConsumptionLevel2[t] = dynamicWriteEnergyConsumptionLevel2 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel2Var) * dynamicWriteEnergyConsumptionLevel2;
                
              } else {
                blk->dynamicWriteEnergyConsumptionLevel2[t] = dynamicWriteEnergyConsumptionLevel2 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel2Var) * dynamicWriteEnergyConsumptionLevel2;
              }

              //dynamicWriteEnergyConsumptionLevel3
              if ((rand()%100) >= 50) {
                blk->dynamicWriteEnergyConsumptionLevel3[t] = dynamicWriteEnergyConsumptionLevel3 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel3Var) * dynamicWriteEnergyConsumptionLevel3;
                
              } else {
                blk->dynamicWriteEnergyConsumptionLevel3[t] = dynamicWriteEnergyConsumptionLevel3 + (((double) (rand() % 100) / 100) * dynamicWriteEnergyConsumptionLevel3Var) * dynamicWriteEnergyConsumptionLevel3;
              }
            }
            //AMHM End
        }
    }
cout<<name()<<"\n";
printf("dynamicReadEnergyConsumptionLevel0: %le\n", dynamicReadEnergyConsumptionLevel0);
printf("dynamicWriteEnergyConsumptionLevel0:%le\n", dynamicWriteEnergyConsumptionLevel0);
printf("dynamicReadEnergyConsumptionLevel1: %le\n", dynamicReadEnergyConsumptionLevel1);
printf("dynamicWriteEnergyConsumptionLevel1:%le\n", dynamicWriteEnergyConsumptionLevel1);
printf("dynamicReadEnergyConsumptionLevel2: %le\n", dynamicReadEnergyConsumptionLevel2);
printf("dynamicWriteEnergyConsumptionLevel2:%le\n", dynamicWriteEnergyConsumptionLevel2);
printf("dynamicReadEnergyConsumptionLevel3: %le\n", dynamicReadEnergyConsumptionLevel3);
printf("dynamicWriteEnergyConsumptionLevel3:%le\n", dynamicWriteEnergyConsumptionLevel3);
cout<<"#################################################################################\n";
}

BaseSetAssoc::~BaseSetAssoc()
{
    delete [] dataBlks;
    delete [] blks;
    delete [] sets;
}

CacheBlk*
BaseSetAssoc::findBlock(Addr addr, bool is_secure) const
{
    Addr tag = extractTag(addr);
    unsigned set = extractSet(addr);
    BlkType *blk = sets[set].findBlk(tag, is_secure);
    return blk;
}

CacheBlk*
BaseSetAssoc::findBlockBySetAndWay(int set, int way) const
{
    return sets[set].blks[way];
}

void*
BaseSetAssoc::wayAccessAnalysis(unsigned int reliabilityLevel, CacheBlk *blk) const
{
	switch (reliabilityLevel) {
	    case 0 : sets[blk->set].L0Accesses++;
	    		 sets[blk->set].totalAccesses++;
	    		 cacheWay[blk->way].L0Accesses++;
	    		 cacheWay[blk->way].totalAccess++;
	    		 break;
	    case 1 : sets[blk->set].L1Accesses++;
	    		 sets[blk->set].totalAccesses++;
	    		 cacheWay[blk->way].L1Accesses++;
				 cacheWay[blk->way].totalAccess++;
	    		 break;
	    case 2 : sets[blk->set].L2Accesses++;
	    		 sets[blk->set].totalAccesses++;
	    		 cacheWay[blk->way].L2Accesses++;
				 cacheWay[blk->way].totalAccess++;
	    		 break;
	    case 3 : sets[blk->set].L3Accesses++;
	    		 sets[blk->set].totalAccesses++;
	    		 cacheWay[blk->way].L3Accesses++;
				 cacheWay[blk->way].totalAccess++;
	    		 break;
	    default : break;
	}
	return 0;
}

void
BaseSetAssoc::wayAccessAnalysisOutput()
{
	double averageWayL0Ratio = 0;
	double averageWayL1Ratio = 0;
	double averageWayL2Ratio = 0;
	double averageWayL3Ratio = 0;

	if(name()=="system.l2.tags")
	{
		std::cout<< outdir<<std::endl;
		FILE * setAccess;
		FILE * wayAccess;
		char filename0[160];
		snprintf(filename0,160,"%s/setAccessAnalysis.csv",outdir.data());
		std::cout<<filename0<<std::endl;
		setAccess = fopen(filename0,"a");
		if (setAccess==NULL) {
		    printf("The set analysis file could not be opened!\n");
		    return;
		}else{
			fprintf(setAccess,"\n\n");
			fprintf(setAccess," ,");
			for (unsigned i = 0; i < numSets; ++i)
				fprintf(setAccess,"%d,",i);
			fprintf(setAccess,"\n");
			fprintf(setAccess,"L0,");
			for (unsigned i = 0; i < numSets; ++i){
				if(sets[i].totalAccesses == 0){
					fprintf(setAccess,"NO_ACCESS,");
					continue;
				}
				fprintf(setAccess,"%f,",(double) sets[i].L0Accesses / sets[i].totalAccesses);
			}
			fprintf(setAccess,"\n");
			fprintf(setAccess,"L1,");
			for (unsigned i = 0; i < numSets; ++i){
				if(sets[i].totalAccesses == 0){
					fprintf(setAccess,"NO_ACCESS,");
					continue;
				}
				fprintf(setAccess,"%f,",(double) sets[i].L1Accesses / sets[i].totalAccesses);
			}
			fprintf(setAccess,"\n");
			fprintf(setAccess,"L2,");
			for (unsigned i = 0; i < numSets; ++i){
				if(sets[i].totalAccesses == 0){
					fprintf(setAccess,"NO_ACCESS,");
					continue;
				}
				fprintf(setAccess,"%f,",(double) sets[i].L2Accesses / sets[i].totalAccesses);
			}
			fprintf(setAccess,"\n");
			fprintf(setAccess,"L3,");
			for (unsigned i = 0; i < numSets; ++i){
				if(sets[i].totalAccesses == 0){
					fprintf(setAccess,"NO_ACCESS,");
					continue;
				}
				fprintf(setAccess,"%f,",(double) sets[i].L3Accesses / sets[i].totalAccesses);
			}
			fprintf(setAccess,"\n");
			for (unsigned i = 0; i < numSets; ++i){
				sets[i].L0Accesses = 0;
				sets[i].L1Accesses = 0;
				sets[i].L2Accesses = 0;
				sets[i].L3Accesses = 0;
				sets[i].totalAccesses = 0;
			}
			fclose(setAccess);
		}
		char filename1[160];
		snprintf(filename1,160,"%s/wayAccessAnalysis.csv",outdir.data());
		std::cout<<filename1<<std::endl;
		wayAccess = fopen(filename1,"a");
		if (wayAccess==NULL) {
			printf("The way analysis file could not be opened!\n");
			return;
		}else{
			fprintf(wayAccess,"\n\n");
			fprintf(wayAccess," ,");
			for (unsigned i = 0; i < assoc; ++i)
				fprintf(wayAccess,"%d,",i);
			fprintf(wayAccess,"\n");
			fprintf(wayAccess,"L0,");
			for (unsigned i = 0; i < assoc; ++i){
				if(cacheWay[i].totalAccess == 0){
					fprintf(wayAccess,"NO_ACCESS,");
					continue;
				}
				averageWayL0Ratio += (double) cacheWay[i].L0Accesses / cacheWay[i].totalAccess;
				fprintf(wayAccess,"%f,",(double) cacheWay[i].L0Accesses / cacheWay[i].totalAccess);
			}
			fprintf(wayAccess,"\n");
			fprintf(wayAccess,"L1,");
			for (unsigned i = 0; i < assoc; ++i){
				if(cacheWay[i].totalAccess == 0){
					fprintf(wayAccess,"NO_ACCESS,");
					continue;
				}
				averageWayL1Ratio += (double) cacheWay[i].L1Accesses / cacheWay[i].totalAccess;
				fprintf(wayAccess,"%f,",(double) cacheWay[i].L1Accesses / cacheWay[i].totalAccess);
			}
			fprintf(wayAccess,"\n");
			fprintf(wayAccess,"L2,");
			for (unsigned i = 0; i < assoc; ++i){
				if(cacheWay[i].totalAccess == 0){
					fprintf(wayAccess,"NO_ACCESS,");
					continue;
				}
				averageWayL2Ratio += (double) cacheWay[i].L2Accesses / cacheWay[i].totalAccess;
				fprintf(wayAccess,"%f,",(double) cacheWay[i].L2Accesses / cacheWay[i].totalAccess);
			}
			fprintf(wayAccess,"\n");
			fprintf(wayAccess,"L3,");
			for (unsigned i = 0; i < assoc; ++i){
				if(cacheWay[i].totalAccess == 0){
					fprintf(wayAccess,"NO_ACCESS,");
					continue;
				}
				averageWayL3Ratio += (double) cacheWay[i].L3Accesses / cacheWay[i].totalAccess;
				fprintf(wayAccess,"%f,",(double) cacheWay[i].L3Accesses / cacheWay[i].totalAccess);
			}
			fprintf(wayAccess,"\n");
			for (unsigned i = 0; i < assoc; ++i){
				cacheWay[i].L0Accesses = 0;
				cacheWay[i].L1Accesses = 0;
				cacheWay[i].L2Accesses = 0;
				cacheWay[i].L3Accesses = 0;
				cacheWay[i].totalAccess = 0;
			}
			if(averageL0Access.zero())
					averageL0Access = (double) averageWayL0Ratio / assoc;
			else {
				if(averageWayL0Ratio != 0)
					averageL0Access = (averageL0Access.value() + ((double) averageWayL0Ratio / assoc)) / 2;
			}

			if(averageL1Access.zero())
				averageL1Access = (double) averageWayL1Ratio / assoc;
			else {
				if(averageWayL1Ratio != 0)
					averageL1Access = (averageL1Access.value() + ((double) averageWayL1Ratio / assoc)) / 2;
			}

			if(averageL2Access.zero())
				averageL2Access = (double) averageWayL2Ratio / assoc;
			else {
				if(averageWayL2Ratio != 0)
					averageL2Access = (averageL2Access.value() + ((double) averageWayL2Ratio / assoc)) / 2;
			}

			if(averageL3Access.zero())
				averageL3Access = (double) averageWayL3Ratio / assoc;
			else {
				if(averageWayL3Ratio != 0)
					averageL3Access = (averageL3Access.value() + ((double) averageWayL3Ratio / assoc)) / 2;
			}
			fclose(wayAccess);
		}
	}
}

std::string
BaseSetAssoc::print() const {
    std::string cache_state;
    for (unsigned i = 0; i < numSets; ++i) {
        // link in the data blocks
        for (unsigned j = 0; j < assoc; ++j) {
            BlkType *blk = sets[i].blks[j];
            if (blk->isValid())
                cache_state += csprintf("\tset: %d block: %d %s\n", i, j,
                        blk->print());
        }
    }
    if (cache_state.empty())
        cache_state = "no valid tags\n";
    return cache_state;
}

void
BaseSetAssoc::cleanupRefs()
{
    for (unsigned i = 0; i < numSets*assoc; ++i) {
        if (blks[i].isValid()) {
            totalRefs += blks[i].refCount;
            ++sampledRefs;
        }
    }
}

void
BaseSetAssoc::computeStats()
{
    for (unsigned i = 0; i < ContextSwitchTaskId::NumTaskId; ++i) {
        occupanciesTaskId[i] = 0;
        for (unsigned j = 0; j < 5; ++j) {
            ageTaskId[i][j] = 0;
        }
    }

    for (unsigned i = 0; i < numSets * assoc; ++i) {
        if (blks[i].isValid()) {
            assert(blks[i].task_id < ContextSwitchTaskId::NumTaskId);
            occupanciesTaskId[blks[i].task_id]++;
            assert(blks[i].tickInserted <= curTick());
            Tick age = curTick() - blks[i].tickInserted;

            int age_index;
            if (age / SimClock::Int::us < 10) { // <10us
                age_index = 0;
            } else if (age / SimClock::Int::us < 100) { // <100us
                age_index = 1;
            } else if (age / SimClock::Int::ms < 1) { // <1ms
                age_index = 2;
            } else if (age / SimClock::Int::ms < 10) { // <10ms
                age_index = 3;
            } else
                age_index = 4; // >10ms

            ageTaskId[blks[i].task_id][age_index]++;
        }
    }
}
//AMHM End
