//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Jon Tokad";
const char *studentID   = "00333-46696";
const char *email       = "jtokad@purdue.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[5] = { "Static", "Gshare",
                          "Tournament", "Custom", "Bimodal" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int bhistoryBits; // Number of bits used for Bimodal History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//

#define BHT_SIZE (4096)
//uint8_t bht[BHT_SIZE];
uint8_t *bht;
uint32_t ghr;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Initialize the BHT
void
init_bht(int size)
{

  // Figure out how many bits needed to index into BHT
  unsigned int tmp = size;
  bhistoryBits = 0;
  while (tmp >>= 1) bhistoryBits++;

  bht = (uint8_t *) malloc(size * sizeof(uint8_t));

  // initialize the bht to all weakly not taken
  for (int i = 0; i < size; i++) {
    bht[i] = 1;
  }
}

// Initialize the predictor
//
void
init_predictor()
{
  //
  //TODO: Initialize Branch Predictor Data Structures
  //
  switch (bpType) {
    case STATIC:
    case BIMODAL:

      init_bht(BHT_SIZE);
      break;

    case GSHARE:

      init_bht(1 << ghistoryBits);

      // global history register init

      ghr = 0;

      break;

    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
  //
  //TODO: Implement prediction scheme
  //
  uint32_t idx;


  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
      return TAKEN;
    case BIMODAL:

      // TODO

      // use last (bhistoryBits) bits to get the index of BHT
      idx = pc & ((1 << bhistoryBits) - 1);
      //printf("%d\n", idx);

      if (bht[idx] == 0 || bht[idx] == 1) return NOTTAKEN;
      else if (bht[idx] == 2 || bht[idx] == 3) return TAKEN;
      else return NOTTAKEN;

      break;

    case GSHARE:

      // TODO

      idx = (pc ^ ghr) & ((1 << ghistoryBits) - 1);
      //printf("%d\n", idx);
//printf("GHR: %d Masked PC: %d XOR Result: %d\n", ghr, (pc & ((1 << ghistoryBits - 1))), idx);
      if (bht[idx] == 0 || bht[idx] == 1) return NOTTAKEN;
      else if (bht[idx] == 2 || bht[idx] == 3) return TAKEN;
      else return NOTTAKEN;

      break;

    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void
train_predictor(uint32_t pc, uint8_t outcome)
{
  //
  //TODO: Implement Predictor training
  //

  uint32_t idx;

  switch (bpType) {
    case STATIC:
    case BIMODAL:

      // ensure we don't go lower or higher than strongly (mis)predicted!
      idx = pc & ((1 << bhistoryBits) - 1);
      if (outcome == NOTTAKEN && bht[idx] > 0) bht[idx] -= 1;
      else if (outcome == TAKEN && bht[idx] < 3) bht[idx] += 1;

      break;

    case GSHARE:

      // update ghr first
      // surround with masks
       //printf("%d ^ %d = %d\n", ghr, getIdx(pc), idx);

      idx = ghr ^ (pc & ((1 << ghistoryBits) - 1));
      //printf("%d\n", idx);
      if (outcome == NOTTAKEN && bht[idx] > 0) bht[idx] -= 1;
      else if (outcome == TAKEN && bht[idx] < 3) bht[idx] += 1;

      ghr &= (1 << ghistoryBits) - 1;
      ghr <<= 1;
      if (outcome == TAKEN) ghr |= 0x1;
      ghr &= (1 << ghistoryBits) - 1;


      break;

    case TOURNAMENT:
    case CUSTOM:
    default:
      break;
  }
}
