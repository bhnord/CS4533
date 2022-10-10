; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

declare i8* @printf(...)

define i32 @main(i32 %0, i8** %1) {
entry:
  ret i32 0
}

define void @t(i32 %k, i32 %v) {
thead:
  br label %block

block:                                            ; preds = %thead
  %s = alloca i32, align 4
  store i32 0, i32* %s, align 4
  %s1 = load i32, i32* %s, align 4
  %0 = icmp eq i32 %s1, 1
  br i1 %0, label %then, label %else

then:                                             ; preds = %block
  br label %block3

cont:                                             ; preds = %block3, %block2
  ret i32 0

else:                                             ; preds = %block
  br label %block2

block2:                                           ; preds = %else
  %x = alloca i32, align 4
  store i32 0, i32* %x, align 4
  br label %cont

block3:                                           ; preds = %then
  %k4 = alloca i32, align 4
  store i32 0, i32* %k4, align 4
  br label %cont
}
