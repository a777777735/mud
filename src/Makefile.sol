CC        = gcc
#PROF     = -pg
DEBUG     =
PIPE      = -pipe
WARNING   = -Wall
O_FLAGS   = -O -fomit-frame-pointer -Wstrict-prototypes
C_FLAGS   = $(O_FLAGS) $(DEBUG) $(PROF) $(WARNING) $(PIPE)
L_FLAGS   = $(O_FLAGS) $(DEBUG) $(PROF) -lelf -lsocket -lnsl -lthread

O_FILES  = act_comm.o act_info.o act_move.o act_obj.o act_wiz.o alias.o \
           angel.o ansi.o arena.o auction.o automap.o badfriend.o bit.o \
           board.o body.o bounty.o buffer.o bus.o chance.o check.o class.o \
           club.o comm.o database.o db.o deposit.o edit.o enable.o enemy.o \
           enroll.o event.o ex_spell.o fcntl.o ff7.o fight.o file.o free.o \
           friend.o fs.o function.o gamble.o gift.o handler.o hero.o hire.o \
           ini.o interp.o ipc.o job.o limit.o load.o lotto.o magic.o make.o \
           mem.o mob_comm.o mob_prog.o mount.o net.o object.o quest.o \
           question.o random.o reload.o routine.o save.o ship.o signal.o \
           skill.o scan.o script.o socket.o special.o spell.o spirit.o \
           stock.o string.o system.o tick.o top.o update.o wanted.o wedding.o \
           wizard.o variable.o vote.o

merc: $(O_FILES)
	rm -f merc
	$(CC) $(L_FLAGS) -o merc $(O_FILES)

clean:
	@echo "[1;33mCleaning all object files and merc[0m"
	@rm -f *.o merc

.c.o: merc.h
	$(CC) -c $(C_FLAGS) $<
