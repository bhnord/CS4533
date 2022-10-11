; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@k = common global i32 0, align 4

declare i32 @blo(i32, i32)

define i32 @callme() {
callmehead:
  br label %block

block:                                            ; preds = %callmehead
  store i32 10, i32* @k, align 4
  %k = load i32, i32* @k, align 4
  ret i32 %k
}

define i32 @c(i32 %f) {
chead:
  %f1 = alloca i32, align 4
  store i32 %f, i32* %f1, align 4
  br label %block

block:                                            ; preds = %chead
  %0 = call i32 @blo(i32 3, i32 1)
  ret i32 %0
}
