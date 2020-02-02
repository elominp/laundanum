const lrt = require('./ng_lrt'); // Import C++ .node object
lrt.initLibrary('666'); // Call C function `int initLibrary(const char *);`
// Create a new C++ CDelivery_Receipt_Instruction instance:
dri = new lrt.DeliveryReceiptInstruction();
dri.setMember('DRI_CODE', '12345678'); // call C++ SetMember method
dri.setMember('DRI_LINK', 'A001'); // call C++ SetMember method
dri.load(); // call C++ COM_Select method
ext = dri.getExtension(); // call C++ getExtension method
status = dri.getMember('IL_STA_ID'); // call C++ sGetMember method
dri.create(); // call C++ SIB engine (init, phases 1, 2, 3, ...etc)