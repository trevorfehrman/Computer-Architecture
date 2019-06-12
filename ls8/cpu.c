#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cpu.h"

#define DATA_LEN 6
#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))

unsigned char cpu_ram_read(struct cpu *cpu, int index)
{
  return cpu->ram[index];
}

void cpu_ram_write(struct cpu *cpu, int index, unsigned char value)
{
  cpu->ram[index] = value;
}

/**
 * Load the binary bytes from a .ls8 source file into a RAM array
 */
void cpu_load(struct cpu *cpu, char *file)
{
  char line[1024];
  int address = 0;
  FILE *fp = fopen(file, "r");

  if (fp == NULL)
  {
    fprintf(stderr, "file not found\n");
    exit(1);
  }

  while (fgets(line, 1024, fp) != NULL)
  {
    char *endptr;

    unsigned char v = strtoul(line, &endptr, 2);

    if (endptr == line)
    {
      continue;
    }

    cpu->ram[address] = v;
    address++;
  }

  fclose(fp);
}

/**
 * ALU
 */
void alu(struct cpu *cpu, enum alu_op op, unsigned char regA, unsigned char regB)
{
  switch (op)
  {
  case ALU_MUL:
    cpu->registers[regA] = cpu->registers[regA] * cpu->registers[regB];
    break;

  case ALU_ADD:
    cpu->registers[regA] = cpu->registers[regA] + cpu->registers[regB];
    break;

  case ALU_CMP:
    if (cpu->registers[regA] == cpu->registers[regB])
    {
      // cpu->fl = cpu->fl | 00000001;
      cpu->fl = cpu->fl | 1;
    }
    else if (cpu->registers[regA] > cpu->registers[regB])
    {
      // cpu->fl = cpu->fl | 00000010;
      cpu->fl = cpu->fl | 2;
    }
    else
    {
      // cpu->fl = cpu->fl | 00000100;
      cpu->fl = cpu->fl | 4;
    }

    break;
  default:
    printf("something has gone terribly wrong");

    // TODO: implement more ALU ops
  }
}

/**
 * Run the CPU
 */
void cpu_run(struct cpu *cpu)
{
  unsigned char first_operand;
  unsigned char second_operand;
  int operand_count;
  int running = 1; // True until we get a HLT instruction

  while (running)
  {
    // TODO
    // 1. Get the value of the current instruction (in address PC).
    cpu->ir = cpu_ram_read(cpu, cpu->pc);

    cpu->pc++;
    // 2. Figure out how many operands this next instruction requires
    if (CHECK_BIT(cpu->ir, 7))
    {
      operand_count = 2;
    }
    else if (CHECK_BIT(cpu->ir, 6))
    {
      operand_count = 1;
    }
    else
    {
      operand_count = 0;
    }
    // 3. Get the appropriate value(s) of the operands following this instruction

    if (operand_count == 2)
    {
      first_operand = cpu_ram_read(cpu, cpu->pc);
      cpu->pc++;
      second_operand = cpu_ram_read(cpu, cpu->pc);
      cpu->pc++;
    }
    else if (operand_count == 1)
    {
      first_operand = cpu_ram_read(cpu, cpu->pc);
      cpu->pc++;
    }
    // 4. switch() over it to decide on a course of action.

    switch (cpu->ir)
    {

    case LDI:
      cpu->registers[first_operand] = second_operand;
      break;

    case PRN:
      printf("%d\n", cpu->registers[first_operand]);
      break;

    case HLT:
      running = 0;
      break;

    case MUL:
      alu(cpu, ALU_MUL, first_operand, second_operand);
      break;

    case ADD:
      alu(cpu, ALU_ADD, first_operand, second_operand);
      break;

    case PUSH:
      cpu->registers[7] -= 1;
      cpu->ram[cpu->registers[7]] = cpu->registers[first_operand];
      break;

    case POP:
      cpu->registers[first_operand] = cpu->ram[cpu->registers[7]];
      cpu->registers[7] += 1;
      break;

    case CALL:
      cpu->registers[7] -= 1;
      cpu->ram[cpu->registers[7]] = cpu->pc;
      cpu->pc = cpu->registers[first_operand];
      break;

    case RET:
      cpu->pc = cpu->ram[cpu->registers[7]];
      cpu->registers[7] += 1;
      break;

    case JMP:
      cpu->pc = cpu->registers[first_operand];
      break;

    case CMP:
      alu(cpu, ALU_CMP, first_operand, second_operand);
      break;

    case JLE:
      if (CHECK_BIT(cpu->fl, 2) || (CHECK_BIT(cpu->fl, 0)))
      {
        cpu->pc = cpu->registers[first_operand];
      }
      cpu->fl = cpu->fl & 00000000;
      break;

    case PRA:
      printf("%c", cpu->registers[first_operand]);
      break;

    case INC:
      cpu->registers[first_operand] += 1;
      break;

    default:
      printf("BLALALLLARRHRGHGHGHH!!");
      running = 0;
    }
    // 5. Do whatever the instruction should do according to the spec.
    // 6. Move the PC to the next instruction.
  }
}

/**
 * Initialize a CPU struct
 */
void cpu_init(struct cpu *cpu)
{
  // TODO: Initialize the PC and other special registers
  cpu->pc = 0;
  cpu->ir = 0;
  cpu->mar = 0;
  cpu->mdr = 0;
  cpu->fl = 0;

  memset(cpu->registers, 0, 6);
  cpu->registers[7] = 0xF4;
  memset(cpu->ram, 0, 256);
}
