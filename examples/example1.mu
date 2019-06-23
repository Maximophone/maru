let main = fn(){
    printl("This is a Maru program");
    printl();
    for(_ in range(5)){
        printl("Maru supports loops!");
    }

    printl("But how fast are Maru's loops exactly?");

    printl("Let's iterate 10M times.");
    let x = range(10000000);
    printl("created range.");
    for(_ in x){

    }
    printl("Done.");
};

main();