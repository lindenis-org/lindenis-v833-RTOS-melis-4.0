# ARMV5TE设置system模式的原因
在ARM v4以后的版本里，ARM核都有7中工作模式：User, FIQ, IRQ, Supervisor, Abort, Undefined,和System。这7中processor mode又分为3类：User mode, Privileged mode和system mode。对于System mode的作用一直有些模糊，今天在arm的网站上找到了一份说明文档，对这个问题的解析很到位，特记录如下：
5.12. System mode
The ARM Architecture defines a User mode that has 15 general purpose registers, a pc, and a CPSR. In addition to this mode there are five privileged processor modes, each of which have an SPSR and a number of registers that replace some of the 15 User mode general purpose registers. 

Note
This section only applies to processors that implement ARM architectures v4, v4T and later.

When a processor exception occurs, the current program counter is copied into the link register for the exception mode, and the CPSR is copied into the SPSR for the exception mode. The CPSR is then altered in an exception-dependent way, and the program counter is set to an exception-defined address to start the exception handler.

The ARM subroutine call instruction (BL) copies the return address into r14 before changing the program counter, so the subroutine return instruction moves r14 to pc (MOV pc,lr).

Together these actions imply that ARM modes that handle exceptions must ensure that another exception of the same type cannot occur if they call subroutines, because the subroutine return address will be overwritten with the exception return address.(In earlier versions of the ARM architecture, this problem has been solved by either carefully avoiding subroutine calls in exception code, or changing from the privileged mode to User mode.The first solution is often too restrictive, and the second means the task may not have the privileged access it needs to run correctly.)

ARM architecture v4 and later provide a processor mode called system mode, to overcome this problem. System mode is a privileged processor mode that shares the User mode registers. Privileged mode tasks can run in this mode, and exceptions no longer overwrite the link register.


Note
System mode cannot be entered by an exception. The exception handlers modify the CPSR to enter System mode. See Reentrant interrupt handlers for an example.
# support-sw of arm offical  support
-  Please quote reference number TAC710123 when referring to this issue.

Hi ,

    >  Is it would be safe if "swi" be called during the SVC mode (nested swi) on ARM926EJ-S core?  i need
    >  to do this on my rtos project, which running on SVC mode totally, but user application need to get the
    >  kernel service by "swi" calling, thanks for  your help.

It is safe but it has the same semantics while within the same mode as any branch with link; taking an
exception to SVC mode will modify LR_svc with the return address for the exception. If the SWI
instruction executed in SVC mode then the value of LR_svc it may rely on for correct operation will be
lost in favor of the exception return address.

You can simply save the state of LR across the call by pushing it to the stack or some alternative
means, and restore it after the SWI instruction if it returns.

Hi caozilong,

    > and thanks for your kindly replay for  assurance me,  so, seems it should be work  if i write as
    > below  in my pritical project.
    >
    > which lr is in the destory part, so it wold be ganautee keep the same after the system call, is that
    > right?
    >
    >
    >           #define __os_syscall(...) do { \
    >               __asm__ __volatile__ ( "svc 0" \
    >                   : "=r"(r0) : __VA_ARGS__ : "memory", "lr"); \
    >             return r0; \
    >         } while (0)

You may be able to do that; it should be saving LR somewhere before the SVC instruction and later doing
something appropriate to pull it back out.

You'll have to figure out the corner cases here, but you will also really have to ensure that SPSR_svc
is covered, too. Since SPSR_svc is copied to CPSR on exception return, the exception entry must match
the exception return state (T, NCVZ, I, F, A). That is to say, you can only get away with not saving
SPSR_svc as well if SPSR_svc is basically constant across the syscall. I think this would be pretty hard
to manage, so it'll be easier to do it manually.

I am not sure you can use SPSR as a clobber and have the inline assembler work out what to do, since it's
a system register. You may have to use a discrete function call in assembler.

    >  Thanks to your suggests, it is hard for me to think about this.  just as the corner situation you
    > have told me,  if the storage and recover operation was done before and after the "svc",  can this work
    > for the corner situation now?  although seems more or less low effenctiy,

Yes, it's a little overzealous (you don't need to save CPSR).

The corner case is this. You can have two SVC kinds, one which the handler returns directly to the
instruction after the SVC, and one where the SVC redirects to some other code as a result of the call
and never directly returns.

So, in the first case:

   SAVE lr, spsr
   SVC 0x0
   RESTORE lr, spsr         <----- LDM pc,[sp]^ or MOVS pc,lr return here

In the second case:

   SAVE lr, spsr
   SVC 0x0                  -----> handler returns to some other code
   RESTORE lr, spsr         -----> this instruction is not met yet

At this point if there's no way to recover where the RESTORE sequence is, you've got two items on the
stack which means any code that makes an assumption about it must be able to return to that place. You
will have to keep track. If you nest handlers at all then you will just fill the stack with those
items in a way you never did before..

I don't know the design of your OS (nor do we want to) so you may not ever have this situation.

    > but the safety is the first import for me, thanks for your kindly help.

We can't give you any real advice on how to implement it, just tell you that SPSR and LR in SVC mode
will be corrupted as a result of SVC instruction, and therefore you must do a little bit of code
coverage and make sure that you are aware of the effects it might have.

Ta!
Matt


Hi,

>
> i am runing a RTOS on ARM926EJS Core on my project, now i want to
> disable the MMU support on it, but i hear about that the cache cant
> be used without enabled the MMU, is this the official saying, or
> just a mistake?
>

When the MMU is disabled, all data accesses will be treated as non-cachable.

Please refer to table 2.12 (Effects of Control Register on caches) in the ARM926EJ-S technical reference manual:

http://infocenter.arm.com/help/topic/com.arm.doc.ddi0198e/I1039296.html#id6975336

Regards,

Martin


Hi,

>
>   So this can be thinked as  the performance would be down if i
> disable MMU on arm926ejs?
>

Yes, it is very likely that running with the data cache disabled will have a big impact on performance.  How big will depend on the application.

If you ARM926EJ-S has TCMs (Tightly Coupled Memories), you could consider using those while the MMU/data cache are disabled.

>
> is this same on other arch, such as cortex-A processor?
>

The ARM926EJ-S's behaviour is very similar to the Cortex-A family.  You need to enable the MMU, and mark the appropriate regions as cacheable, before you can make use of the data cache.

Things are different in the Cortex-R family.  The Cortex-R processors have a MPU instead of a MMU, and use a default region set when the MPU is disabled.

Regards,

Martin

Hi,

>
> Thank you , but did the TCM implementation depends?  on you ARM, or on our digital designer?
>

The ARM926EJ-S includes the ports to connect TCMs, but it is up to the chip designer whether of not to include them.

Software can check whether the instruction and/or data TCM is present by reading the Size field of the TCM region registers:

http://infocenter.arm.com/help/topic/com.arm.doc.ddi0198e/I1002021.html#Bhcbifgi

Alternatively, you could check the documentation from the SoC supplied.


Martin


> ENTRY(__kvm_call_hyp)
> alternative_if_not ARM64_HAS_VIRT_HOST_EXTN
> hvc #0
> ret
>
> but i dont know the KVM running on armv8 belongs to which of the two type,  type 1 or type 2?


KVM is a Type 2 (hosted) hypervisor, but the Armv8.0-A architecture's Virtualization Extensions (EL2) are only really suited to Type 1 (bare metal) hypervisors, which makes implementing KVM awkward.

The way KVM works in Armv8.0-A is by entering the kernel in EL2 and installing a vector table there before dropping to EL1 and continuing the boot (with HCR_EL2.VM=0). To spawn a new virtual machine, the "host" kernel executes a HVC to jump up to the vector table that it installed in EL2, does what it needs to do, and then enters the virtual machine in EL1 with HCR_EL2.VM=1.

Armv8.1-A added the Virtualization Host Extensions (VHE) which are much better suited Type 2 hypervisors by allowing for an operating system to run directly in EL2. This is what the "alternative_if_not ARM64_HAS_VIRT_HOST_EXTN" in your kernel code snippet is referring to.

In Armv8.1-A with VHE, the host kernel boots fully in EL2 and then spawns guest VMs in EL1.


I hope that helps.


Kind regards,
Ash.


>   " l executes a HVC to jump up to the vector table that it installed in EL2, does what it needs to do, "
> ========>
>    so the code instructions of vector table and  the "codes going to do" in EL2 is provided by KVM, or by you ARM?
>    can it be reprogramed, or just a solidified rom code ?


The code running in EL2 is part of the same kernel image that is running at EL1 i.e. the single image spans both ELs, with the KVM part running in EL2 and the kernel proper running in EL1.

You can see the initialisation code for this here:

    https://elixir.free-electrons.com/linux/latest/source/arch/arm64/kernel/head.S#L380

Hope that helps.

Kind regards,
Ash.


