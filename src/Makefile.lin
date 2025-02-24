CC        = gcc
#PROF     = -pg
DEBUG     =
PIPE      = -pipe
WARNING   = -Wall
O_FLAGS   = -O3 -fomit-frame-pointer -Wstrict-prototypes
#O_FLAGS  = -O2 -pg
C_FLAGS   = $(O_FLAGS) $(DEBUG) $(PROF) $(WARNING) $(PIPE)
L_FLAGS   = $(O_FLAGS) $(DEBUG) $(PROF)

O_FILES  = act_comm.o
O_FILES += act_info.o
O_FILES += act_move.o
O_FILES += act_obj.o
O_FILES += act_wiz.o
O_FILES += alias.o
O_FILES += angel.o
O_FILES += ansi.o
O_FILES += arena.o
O_FILES += auction.o
O_FILES += automap.o
O_FILES += badfriend.o
O_FILES += bit.o
O_FILES += board.o
O_FILES += body.o
O_FILES += bounty.o
O_FILES += buffer.o
O_FILES += bus.o
O_FILES += chance.o
O_FILES += check.o
O_FILES += class.o
O_FILES += club.o
O_FILES += comm.o
O_FILES += database.o
O_FILES += db.o
O_FILES += deposit.o
O_FILES += edit.o
O_FILES += enable.o
O_FILES += enemy.o
O_FILES += enroll.o
O_FILES += event.o
O_FILES += ex_spell.o
O_FILES += fcntl.o
O_FILES += ff7.o
O_FILES += fight.o
O_FILES += file.o
O_FILES += free.o
O_FILES += friend.o
O_FILES += fs.o
O_FILES += function.o
O_FILES += gamble.o
O_FILES += gift.o
O_FILES += handler.o
O_FILES += hero.o
O_FILES += hire.o
O_FILES += ini.o
O_FILES += interp.o
O_FILES += ipc.o
O_FILES += job.o
O_FILES += limit.o
O_FILES += load.o
O_FILES += lotto.o
O_FILES += magic.o
O_FILES += make.o
O_FILES += mem.o
O_FILES += mob_comm.o
O_FILES += mob_prog.o
O_FILES += mount.o
O_FILES += net.o
O_FILES += object.o
O_FILES += quest.o
O_FILES += question.o
O_FILES += random.o
O_FILES += reload.o
O_FILES += routine.o
O_FILES += save.o
O_FILES += ship.o
O_FILES += signal.o
O_FILES += skill.o
O_FILES += scan.o
O_FILES += script.o
O_FILES += socket.o
O_FILES += special.o
O_FILES += spell.o
O_FILES += spirit.o
O_FILES += stock.o
O_FILES += string.o
O_FILES += system.o
O_FILES += tick.o
O_FILES += top.o
O_FILES += update.o
O_FILES += wanted.o
O_FILES += wedding.o
O_FILES += wizard.o
O_FILES += variable.o
O_FILES += vote.o

merc: $(O_FILES)

	@rm -f merc
	@echo "[1;33mLinkng Merc ... [0m"
	$(CC) $(L_FLAGS) -o merc $(O_FILES)

clean:
	@echo "[1;33mCleaning all object files and merc[0m"
	@rm -f *.o merc

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<
