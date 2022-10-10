; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

declare i8* @printf(...)

define i32 @main(i32 %0, i8** %1) {
entry:
  %k = alloca i32, align 4
  store i32 0, i32* %k, align 4
  ret i32 0
}

define i32 @t() {
thead:
  br label %block

block:                                            ; preds = %thead
  %j = alloca i32, align 4
  store i32 1, i32* %j, align 4
  %k = load i32, i32* %k, align 4
  ret i32 2
}
