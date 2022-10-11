; ModuleID = 'WPLC.ll'
source_filename = "WPLC.ll"

@k = common global i32 0, align 4

define i32 @callme() {
callmehead:
  br label %block

block:                                            ; preds = %callmehead
  %si = alloca i32, align 4
  store i32 0, i32* %si, align 4
  %si1 = load i32, i32* %si, align 4
  ret i32 %si1
}

define i32 @c(i32 %f) {
chead:
  %f1 = alloca i32, align 4
  store i32 %f, i32* %f1, align 4
  br label %block

block:                                            ; preds = %chead
  ret i32 4
}
