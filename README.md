# lclass
Advanced Lua object-oriented system framework

# version support
> lua54

# lib-content

|     function/field     |                          parameters                          |                                info                                |
| :--------------------: | :----------------------------------------------------------: | :----------------------------------------------------------------: |
|        newClass        |                        类名（string）                        |                             定义一个类                             |
|     setConstructor     |             类（luaclass）,构建函数（function）              |                 定义这个类的构建函数，只有一次机会                 |
|    setDeconstructor    |      类/对象（luaclass/luaobject）,析构函数（function）      | 如果是类定义类的析构函数，如果是对象定义对象析构函数，只有一次机会 |
| setObjectDeconstructor |            对象（luaobject）,析构函数（function）            |                   定义对象析构函数，只有一次机会                   |
|     setStaticField     |  类/对象（luaclass/luaobject）,可见性修饰（number），键，值  |           定义静态字段，定义之后会记录键值，只有一次机会           |
|        setField        |  类/对象（luaclass/luaobject）,可见性修饰（number），键，值  |           定义实例字段，定义之后会记录键值，只有一次机会           |
|    setStaticMethod     | 类/对象（luaclass/luaobject）,可见性修饰（number），键，函数 |           定义静态方法，定义之后会记录键值，只有一次机会           |
|       setMethod        | 类/对象（luaclass/luaobject）,可见性修饰（number），键，函数 |           定义实例方法，定义之后会记录键值，只有一次机会           |
|        setSuper        | 类/对象（luaclass/luaobject）, 类/对象（luaclass/luaobject） |           定义父类，只有一次机会，类不可以定义对象为父类           |
|        getSuper        |                类/对象（luaclass/luaobject）                 |                      返回父类（无继承为nil）                       |
|        getChild        |                      对象（luaobject）                       |             返回当前对象继承子类对象（叶子对象时nil）              |
|       getMethods       |                类/对象（luaclass/luaobject）                 |                  获取当前类或对象定义的public方法                  |
|       getFields        |                类/对象（luaclass/luaobject）                 |                  获取当前类或对象定义的public字段                  |
|   getDeclaredMethods   |                类/对象（luaclass/luaobject）                 |                   获取当前类或对象定义的所有方法                   |
|   getDeclaredFields    |                类/对象（luaclass/luaobject）                 |                   获取当前类或对象定义的所有字段                   |
|       lockdefine       |                类/对象（luaclass/luaobject）                 |              锁定定义，用于完成构建类或对象后锁定定义              |
|          cast          |               对象（luaobject）,类（luaclass）               |             将当前对象转为目标类的对象（失败抛出错误）             |
|       instanceof       |               对象（luaobject）,类（luaclass）               |           判断当前对象是否（或其父/子类）为目标类的对象            |
|         public         |                              -                               |         可见性修饰，不影响使用，影响后续基于此值设计的函数         |
|        private         |                              -                               |         可见性修饰，不影响使用，影响后续基于此值设计的函数         |

如果你的Lua为魔改版本，可以精确操控CallInfo、编译时准确携带额外上值信息，public和private属性也可以模拟出来（上一CallInfo的func上值包含当前类时判定为类内，允许使用private），不过这很麻烦。
