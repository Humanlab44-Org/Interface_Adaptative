/*
Nom du fichier : index.js
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

var defaultFile;  // nom.json du dernier fichier de touche utilisé
var jsonKeysFile;  // json courant de config de touches 
var specialKeys;   // liste des specialKeys  pour la listbox popup
var jsonKeysFileMaxSize = 4096 // pour le wroom

// ------------------------------------------------  fonctions ---------------------
function init() {
    // initialistion generale
    getSpecialKeys();
    set_listKeysFiles();
}

function set_listKeysFiles() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "listKeysFiles", true);
    xhr.onload = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            defaultFile = document.getElementById("27").value;
            console.log("default file:  " + defaultFile);
            var fichiers = JSON.parse(xhr.responseText);
            var listbox = document.getElementById("listBoxKeysFiles");

            // Suppression de toutes les options
            for (var i = listbox.options.length - 1; i >= 0; i--) {
                listbox.remove(i);
            }

            for (var i = 0; i < fichiers.length; i++) {
                var option = document.createElement("option");
                option.value = fichiers[i];
                option.text = fichiers[i];
                if (option.text == defaultFile) {
                    option.selected = true;
                }
                listbox.add(option);
            }
            readJsonKeysConfsFile(defaultFile, reload);
        }
    };
    xhr.send();
}

function removeKeyFile(this_elem) {
    var selectKeyFile = document.getElementById("listBoxKeysFiles");
    var index = selectKeyFile.selectedIndex;
    var fileName = selectKeyFile.value;

    if (selectKeyFile.length <= 1) {
        alert("on ne peut pas supprimmer ce fichier \n il doit en rester au moins une");
        return;
    }
    if (!confirm("Voulez-vous supprimer\n  le fichier de touches   " + fileName)) {
        return;
    }
    deleteKeyFile(fileName);
    selectKeyFile.remove(index);
    set_listKeysFiles
}

function getSpecialKeys() {
    fetch('/specialKeys.json')
        .then(response => response.text())
        .then(data => {
            var jsonSpecialKeys = JSON.parse(data);
            specialKeys = jsonSpecialKeys.SPECIALS_KEYS;  // la liste est recuperée ici et reste en variable globale
            // console.log(specialKeys);
        });
}


function reload(JsonkeysConfs) {  // lecture du fichier de config des touches

    jsonKeysFile = JSON.parse(JsonkeysConfs);
    console.log("chargement du fichier de config des touches");
    console.log(jsonKeysFile);

    // peuple la listbox des 'BANKS'
    const select = document.getElementById("selectBank");
    // Suppression de toutes les options
    for (let i = select.options.length - 1; i >= 0; i--) {
        select.remove(i);
    }
    // Récupération du tableau de banques
    const banks = jsonKeysFile.BANKS;
    // Parcours de toutes les banques
    for (let i = 0; i < banks.length; i++) {
        // Récupération du nom de la banque
        const bankName = banks[i].name;
        // Création d'une option pour la banque et ajout à la liste déroulante
        const option = document.createElement("option");
        option.value = bankName;
        option.text = bankName;
        select.add(option);
    }
    affKeys(banks[0].name);
}

function readJsonKeysConfsFile(file, callback) {
    var rawFile = new XMLHttpRequest();
    console.log("lecture du fichier de touches: " + file);
    rawFile.overrideMimeType("application/json");
    rawFile.open("GET", ("/keysConfs/" + file), true);
    rawFile.onreadystatechange = function () {
        if (rawFile.readyState === 4 && rawFile.status == "200") {
            callback(rawFile.responseText);
            //console.log("on readstatechange");
        }
    }
    rawFile.send(null);
}

function affKeys(bankNameParam) {  // changement de fichier bank et affichage des commandes

    affRunStop();  // afficheur

    //console.log(bankNameParam);
    // // Récupération du tableau de banques
    const banks = jsonKeysFile.BANKS;

    // // Recherche de la banque correspondant au nom passé en paramètre
    const bank = banks.find((b) => b.name === bankNameParam);

    // Si la banque est trouvée, on itère sur ses touches
    if (bank) {
        const bankName = bank.name;
        console.log("affichage des commandes pour la banque: " + bankName);
        //const bankId = bank.id;
        //console.log(bankId);
        const bankKeys = bank.KEYS;  // json des touches pour cette banque
        //console.log(bankKeys);
        const container = document.getElementById('keysList');
        container.innerHTML = '';   // vide la liste


        const bkn = document.createElement("h3");
        bkn.textContent = "Banque de touche: " + bankName;
        keysList.appendChild(bkn);

        //                  Pour toutes les touches de la banque
        for (let i = 0; i < bankKeys.length; i++) {
            // Récupération des informations de la touche
            const key = bankKeys[i];
            //const keyId = key.id;
            //console.log(keyId);
            affKey(i, key);   // affichage de la touche et de ses commandes


            // const keyCommands = key.commands;  // json des commandes pour cette touche
            // //console.log(keyCommands);
            // //                            Pour toutes les commandes de la touche
            // for (let j = 0; j < keyCommands.length; j++) {
            //     const commande = keyCommands[j];
            //     const keyID = commande.id;
            //     const keyType = commande.type;
            //     const Value = commande.value;
            //     //Affichage des informations de la commande
            //     console.log(` type: ${keyType} - value ${Value}`);

            // }
        }
        affJsonSize();
    } else {
        console.log(`Banque "${bankNameParam}" non trouvée`);
    }

}

function affJsonSize() {
    let jsonString = JSON.stringify(jsonKeysFile);
    let sizeInBytes = new Blob([jsonString]).size + 100;
    console.log(sizeInBytes);
    const C = document.getElementById("JsonSize");
    JsonSize.textContent = sizeInBytes;
    if (sizeInBytes > jsonKeysFileMaxSize) {
        JsonSize.classList.remove("bg-success");
        JsonSize.classList.add("bg-danger");
    } else {
        JsonSize.classList.remove("bg-danger");
        JsonSize.classList.add("bg-success");
    }
}

function affKey(no, key) {     // affichge de la touche et de ses commandes

    // On récupère l'élément HTML où on veut afficher les touches
    const container = document.getElementById('keysList');
    container.style.marginLeft = '5px';

    // const cmdsTitle = document.createElement("h4");
    // cmdsTitle.textContent = "Touches";
    // container.appendChild(cmdsTitle);

    // Récupération des informations de la touche
    const keyName = key.name;
    const keyCommands = key.commands;

    // Création de l'élément HTML pour la touche
    const keyDiv = document.createElement("div");
    keyDiv.classList.add("key");
    keyDiv.innerHTML = `<h4>Touche: ${no + 1}</h4>`;
    // ---------- name
    const keyN = document.createElement("input");
    keyN.style.marginRight = '5px';
    keyN.value = key.name;
    keyN.addEventListener("change", function () { changeKeyName(this, no); });
    console.log("touche " + key.name);
    keyDiv.appendChild(keyN);

    // Création du bouton "Ajouter"
    const addButton = document.createElement("button");
    addButton.style.marginRight = '5px';
    addButton.style.marginBottom = '5px';
    addButton.innerText = "+";
    addButton.addEventListener("click", function () { addCmd(no); });
    // Création du bouton "Supprimer"
    const removeButton = document.createElement("button");
    removeButton.innerText = "-";
    removeButton.addEventListener("click", function () { removeCmd(no); });

    // Parcours de toutes les commandes de la touche
    for (let j = 0; j < keyCommands.length; j++) {
        const cmdDiv = document.createElement("div");
        const commande = document.createElement("input");
        // Récupération des informations de la commande
        const command = keyCommands[j];
        commande.value = command.name;
        const commandType = command.type;
        const commandValue = command.value;
        commande.setAttribute("cmdori", "key");
        commande.setAttribute("nokey", no);
        commande.setAttribute("nocmd", j);
        commande.style.marginRight = '5px';
        commande.type = "inputText";
        commande.addEventListener("dblclick", function () { selectSpecKey(this); });
        commande.addEventListener("change", function () { cmdChange(this, true); });
        cmdDiv.appendChild(commande);
        keyDiv.appendChild(cmdDiv);
        container.appendChild(keyDiv);
        cmdDiv.style.display = "inline-block";
        cmdDiv.style.marginRight = '5px';
    }
    keyDiv.appendChild(addButton);
    keyDiv.appendChild(removeButton);
    container.appendChild(keyDiv);
    container.style.marginLeft = '1px';
}

function addKey() {
    // On récupère l'élément HTML où on veut afficher les touches
    const select = document.getElementById('selectBank');
    var bankName = select.value;
    var index = select.selectedIndex;
    console.log("addKey bank: " + bankName + " index: " + index);

    var newKey =
    {
        name: 'new key',
        sensibility: '50',
        commands: [
            {
                name: '',
                type: '',
                value: ''
            }
        ]

    };
    jsonKeysFile.BANKS[index].KEYS.push(newKey);  // ajoute  à la fin du json
    affKeys(selectBank[index].value)
}

function removeKey() {
    // On récupère l'élément HTML où on veut afficher les touches
    const select = document.getElementById('selectBank');
    var bankName = select.value;
    var index = select.selectedIndex;
    var nbKeys = (jsonKeysFile.BANKS[index].KEYS.length);
    console.log("removeKey bank: " + bankName + " index: " + index + " nbKeys: " + nbKeys);

    if (nbKeys <= 1) {
        alert("on ne peut pas supprimmer cette touche \n il doit en rester au moins une");
        return;
    }
    if (!confirm("Voulez-vous supprimer\n  la  touches: " + nbKeys)) {
        return;
    }
    nbKeys -= 1;
    jsonKeysFile.BANKS[index].KEYS.splice(nbKeys, 1); // supprime la derniere touche de la banque dans le  JSON
    console.log(jsonKeysFile);
    affKeys(selectBank[index].value)
}

function addCmd(noKey) {
    // On récupère l'élément HTML 
    const select = document.getElementById('selectBank');
    var bankName = select.value;
    var index = select.selectedIndex;
    console.log("add Cmd: " + bankName + " index: " + index + " touche " + noKey);
    var newCmd =
    {
        name: '',
        type: '',
        value: ''
    }
    jsonKeysFile.BANKS[index].KEYS[noKey].commands.push(newCmd);  // ajoute  à la fin du json
    affKeys(selectBank[index].value)

}


function removeCmd(noKey) {
    // On récupère l'élément HTML 
    const select = document.getElementById('selectBank');
    var bankName = select.value;
    var index = select.selectedIndex;
    var nbCmd = (jsonKeysFile.BANKS[index].KEYS[noKey].commands.length);
    console.log("remove Cmd: " + bankName + " index: " + index + " touche " + noKey + "nb commandes; " + nbCmd);
    if (nbCmd <= 1) {
        alert("on ne peut pas supprimmer cette commande \n il doit en rester au moins une");
        return;
    }
    if (!confirm("Voulez-vous supprimer\n  la  commande: " + nbCmd)) {
        return;
    }
    nbCmd -= 1;
    jsonKeysFile.BANKS[index].KEYS[noKey].commands.splice(nbCmd, 1); // supprime la derniere commande dans le  JSON
    console.log(jsonKeysFile);
    affKeys(selectBank[index].value)
}

function RunStopAddRemoveCmd(runstop, action) {
    // On récupère l'élément HTML 
    const select = document.getElementById('selectBank');
    var index = select.selectedIndex;
    var nbCmd;
    var newCmd =
    {
        name: '',
        type: '',
        value: ''
    }
    if (action == 'rem') {
        switch (runstop) {
            case 'run':
                nbCmd = jsonKeysFile.PROGRAM.cmd_run.length;
                break;
            case 'stop':
                nbCmd = jsonKeysFile.PROGRAM.cmd_stop.length;
                break;
        }
        console.log("nbcmd " + nbCmd);
        if (nbCmd <= 0) {
            alert("il n'y a rien à supprimer!");
            return;  // on ne fait rien
        }
    }
    nbCmd -= 1;
    switch (runstop) {
        case 'run':
            switch (action) {
                case 'add':
                    console.log("run add");
                    jsonKeysFile.PROGRAM.cmd_run.push(newCmd);  // ajoute  à la fin du json
                    break;
                case 'rem':
                    console.log("run rem");
                    jsonKeysFile.PROGRAM.cmd_run.splice(nbCmd, 1);
                    break;
            }
            break;
        case 'stop':
            switch (action) {
                case 'add':
                    console.log("stop add");
                    jsonKeysFile.PROGRAM.cmd_stop.push(newCmd);  // ajoute  à la fin du json
                    break;
                case 'rem':
                    console.log("stop rem");
                    jsonKeysFile.PROGRAM.cmd_stop.splice(nbCmd, 1);
                    break;
            }
            break;
    }
    //console.log(jsonKeysFile);
    affKeys(selectBank[index].value)
}

function affRunStop() {

    // On récupère l'élément HTML où on veut afficher les touches
    const container = document.getElementById('cmdRunStop');
    container.innerHTML = '';
    // recupere dans le JSON
    const program = jsonKeysFile.PROGRAM;
    // Création des éléments HTML

    const programDiv = document.createElement("div");

    const addRunBtn = document.createElement("button");
    addRunBtn.textContent = "+";
    addRunBtn.addEventListener("click", function () { RunStopAddRemoveCmd("run", "add"); });
    addRunBtn.style.marginRight = '5px';

    const deleteRunBtn = document.createElement("button");
    deleteRunBtn.textContent = "-";
    deleteRunBtn.addEventListener("click", function () { RunStopAddRemoveCmd("run", "rem"); });

    const addStopBtn = document.createElement("button");
    addStopBtn.textContent = "+";
    addStopBtn.addEventListener("click", function () { RunStopAddRemoveCmd("stop", "add"); });
    addStopBtn.style.marginRight = '5px';

    const deleteStopBtn = document.createElement("button");
    deleteStopBtn.textContent = "-";
    deleteStopBtn.addEventListener("click", function () { RunStopAddRemoveCmd("stop", "rem"); });

    const prgDiv = document.createElement("div");
    const select = document.getElementById("listBoxKeysFiles");
    var filename = select.value;
    const programName = document.createElement("h3");
    programName.textContent = "Fichier: " + filename;
    const programComment = document.createElement("input");
    programComment.value = program.comment;
    prgDiv.appendChild(programName);
    //prgDiv.appendChild(programComment);    // TODO   deplacer input avec program.comment au dessus 

    // Parcours des commandes ------- "cmd_run" ------------
    const cmdRunDiv = document.createElement("div");
    const cmdRunTitle = document.createElement("h4");
    cmdRunTitle.textContent = "Commandes Run";
    cmdRunDiv.appendChild(cmdRunTitle);

    for (let i = 0; i < program.cmd_run.length; i++) {
        //const cmd = program.cmd_run[i];
        const cmdDiv = document.createElement("div");
        const cmdValue = document.createElement("input");
        const cmd = program.cmd_run[i];
        //cmdType.value = `Type: ${cmd.type}`;
        cmdValue.value = `${cmd.name}`;
        cmdValue.style.marginRight = '5px';
        cmdValue.type = "inputText";
        cmdValue.setAttribute("cmdori", "run");
        cmdValue.setAttribute("nokey", 0);
        cmdValue.setAttribute("nocmd", i);
        cmdValue.addEventListener("dblclick", function () { selectSpecKey(this); });
        cmdValue.addEventListener("change", function () { cmdChange(this, true); });
        cmdDiv.appendChild(cmdValue);
        cmdRunDiv.appendChild(cmdDiv);
        //cmdDiv.appendChild(runKeysSelect);
        cmdDiv.style.display = "inline-block";
        cmdDiv.style.marginRight = '5px';
    }
    programDiv.appendChild(prgDiv);
    cmdRunDiv.appendChild(addRunBtn);
    cmdRunDiv.appendChild(deleteRunBtn);
    programDiv.appendChild(cmdRunDiv);


    // Parcours des commandes  ---------- "cmd_stop" -----------
    const cmdStopDiv = document.createElement("div");
    const cmdStopTitle = document.createElement("h4");
    cmdStopTitle.textContent = "Commandes Stop";
    cmdStopDiv.appendChild(cmdStopTitle);

    for (let i = 0; i < program.cmd_stop.length; i++) {
        const cmd = program.cmd_stop[i];
        const cmdDiv = document.createElement("div");
        const cmdValue = document.createElement("input");
        //var cmdStopSelect = document.createElement("select");
        //cmdType.textContent = `Type: ${cmd.type}`;
        cmdValue.value = `${cmd.name}`;
        cmdValue.style.marginRight = '5px';
        cmdValue.type = "inputText";
        cmdValue.setAttribute("cmdori", "stop");
        cmdValue.setAttribute("nokey", 0);
        cmdValue.setAttribute("nocmd", i);
        cmdValue.addEventListener("dblclick", function () { selectSpecKey(this); });
        cmdValue.addEventListener("change", function () { cmdChange(this, true); });
        cmdDiv.appendChild(cmdValue);
        cmdStopDiv.appendChild(cmdDiv);
        cmdDiv.style.display = "inline-block";
        cmdDiv.style.marginRight = '5px';

    }
    cmdStopDiv.appendChild(addStopBtn);
    cmdStopDiv.appendChild(deleteStopBtn);
    programDiv.appendChild(cmdStopDiv);
    // Ajout du programme dans la page HTML
    container.appendChild(programDiv);
    container.style.marginLeft = '1px';
}

function addBank(this_elem) {
    var select = null;
    if (this_elem.id == "addBank") {
        select = document.getElementById('selectBank');
        console.log("select " + select.id);
    }
    var saisie;
    var pass = false;
    console.log("ajout " + this_elem);
    do {
        if (pass) alert("La valeur est incorrecte.[a-z][0-9]");
        saisie = prompt("Nom de la nouvelle banque:");
        if (saisie == null) {
            return (false);
        }
        pass = true;
    }
    while (saisie.match(/[^a-zA-Z0-9]/) || saisie == "" || saisie.length > 8)  // controle de saisie
    // ajout de l'option
    select.options[select.length] = new Option(saisie, saisie, false, true);  // ajoute à la fin de la liste et select par defaut
    // ajoute la banque à la fin du json
    var newBank = {
        name: "",
        KEYS: [
            {
                name: 'new key',
                sensibility: '50',
                commands: [
                    {
                        name: '',
                        type: '',
                        value: ''
                    }
                ]
            }
        ]
    };
    newBank.name = saisie;
    jsonKeysFile.BANKS.push(newBank);  // ajoute  à la fin du json
    //console.log(jsonKeysFile);
    //console.log("index:" + select.length);
    affKeys(select[select.length - 1].value);

}
function renameBank(this_elem) {
    var selectBank = document.getElementById("selectBank");
    var index = selectBank.selectedIndex;
    var name = selectBank.options[index].text;

    var newName = prompt("Entrez un nouveau nom pour " + name);

    if (newName !== null) {
        selectBank.options[index].text = newName;
        // Récupération du tableau de banques
        const banks = jsonKeysFile.BANKS;
        // change le nom dand le json
        banks[index].name = newName;
    }
}

function removeBank(this_elem) {
    var selectBank = document.getElementById("selectBank");
    var index = selectBank.selectedIndex;
    var name = selectBank.value;
    const banks = jsonKeysFile.BANKS;
    if (banks.length <= 1) {
        alert("on ne peut pas supprimmer cette banque \n il doit en rester au moins une");
        return;
    }
    if (!confirm("Voulez-vous supprimer\n  la banque de touches:   " + name)) {
        return;
    }
    selectBank.remove(index);

    jsonKeysFile.BANKS.splice(index, 1); // supprime la banque du fichier JSON
    console.log(jsonKeysFile);
    affKeys(selectBank[0].value);
    //reload(jsonKeysFile);
}

function changeKeyName(this_elem, noKey) {
    const select = document.getElementById('selectBank');
    var index = select.selectedIndex;
    console.log("changement keyName");
    console.log("banque index " + index + "value: " + this_elem.value + " No touche: " + noKey);
    jsonKeysFile.BANKS[index].KEYS[noKey].name = this_elem.value;  // modif dans le json
}

function cmdChange(this_elem, bChange) {
    // console.log("***** command change");
    // console.log(this_elem.getAttribute("_value"));
    // console.log(this_elem.value);
    // console.log("--------------");
    if (bChange == true) {   // si true -> valeur saisie dans le champ sinon ca arrive de la listbox et rien à changer
        this_elem.setAttribute("_name", this_elem.value);
        this_elem.setAttribute("_value", this_elem.value);
        if (this_elem.value.length == 1) {
            this_elem.setAttribute("_type", 1);  // caractere unique
        } else if (this_elem.value.length > 1) {
            this_elem.setAttribute("_type", 0);   // chaine de carracteres
        } else {
            this_elem.setAttribute("_type", "");   // vide
            this_elem.setAttribute("_name", "");
            this_elem.setAttribute("_value", "");
        }
    }
    console.log("--------");
    var cmdOri = this_elem.getAttribute("cmdori");
    console.log("cmdori " + cmdOri);
    var noKey = this_elem.getAttribute("nokey");
    console.log("noKey " + noKey);
    var noCmd = this_elem.getAttribute("nocmd");
    console.log("noCmd " + noCmd);
    var name = this_elem.getAttribute("_name");
    console.log("name " + name);
    var type = this_elem.getAttribute("_type");
    console.log("type " + type);
    var value = this_elem.getAttribute("_value");
    console.log("value " + value);
    console.log("--------");

    const select = document.getElementById('selectBank');
    var index = select.selectedIndex;

    switch (cmdOri) {
        case 'run':
            console.log('ori run');
            jsonKeysFile.PROGRAM.cmd_run[noCmd].name = name;  // modif dans le json
            jsonKeysFile.PROGRAM.cmd_run[noCmd].type = type;  // modif dans le json
            jsonKeysFile.PROGRAM.cmd_run[noCmd].value = value;  // modif dans le json
            break;
        case 'stop':
            console.log('ori stop');
            jsonKeysFile.PROGRAM.cmd_stop[noCmd].name = name;  // modif dans le json
            jsonKeysFile.PROGRAM.cmd_stop[noCmd].type = type;  // modif dans le json
            jsonKeysFile.PROGRAM.cmd_stop[noCmd].value = value;  // modif dans le json
            break;
        case 'key':
            //console.log('ori key');
            jsonKeysFile.BANKS[index].KEYS[noKey].commands[noCmd].name = name;  // modif dans le json
            jsonKeysFile.BANKS[index].KEYS[noKey].commands[noCmd].type = type;  // modif dans le json
            jsonKeysFile.BANKS[index].KEYS[noKey].commands[noCmd].value = value;  // modif dans le json
            console.log(JSON.stringify(jsonKeysFile.BANKS[index].KEYS[noKey].commands[noCmd]));
            break;
    }
    affJsonSize();
    console.log(JSON.stringify(jsonKeysFile));
}

// ---------------------- popup selection specials keys (listbox)  cloné  temporairement
function selectSpecKey(this_elem) {
    //const selectTmp = document.getElementById("specialKeys");
    //var listBox = document.getElementById("specialKeys");
    var selectTmp = document.createElement("select");
    //select = listBox.cloneNode(true);
    selectTmp.name = "selectTmp";
    selectTmp.id = "selectTmp";
    selectTmp.size = 5;
    var option = document.createElement("option");
    option.text = "selectionnez une commande";
    selectTmp.add(option);

    for (let i = 0; i < specialKeys.length; i++) {
        const option = document.createElement("option");
        option.text = specialKeys[i].name;
        option.setAttribute("_name", specialKeys[i].name);
        option.setAttribute("_type", specialKeys[i].type);
        option.setAttribute("_value", specialKeys[i].value);
        selectTmp.add(option);
    }

    // console.log("///////////");
    // console.log(selectTmp);

    var mouse_x = this_elem.offsetLeft;
    var mouse_y = this_elem.offsetTop;  // recupere la position de la souris
    //console.log("mousex: " + mouse_x + " mouse_y: " + mouse_y);

    var promptDiv = document.createElement("div");
    promptDiv.appendChild(selectTmp);
    promptDiv.style.position = "absolute";
    // promptDiv.style.top = "50%";
    // promptDiv.style.left = "50%";
    promptDiv.style.top = mouse_y - 20 + "px";
    promptDiv.style.left = mouse_x + "px"; // ajouter 20 pixels pour décaler le popup sur la droite
    //promptDiv.style.transform = "translate(-50%, -50%)";
    promptDiv.style.backgroundColor = "#fff";
    promptDiv.style.padding = "20px";
    promptDiv.style.borderRadius = "10px";
    document.body.appendChild(promptDiv);

    selectTmp.onchange = function () {
        var selectedOption = selectTmp.options[selectTmp.selectedIndex];
        this_elem.value = selectedOption.getAttribute("_name");
        this_elem.setAttribute("_name", selectedOption.getAttribute("_name"));
        this_elem.setAttribute("_type", selectedOption.getAttribute("_type"));
        this_elem.setAttribute("_value", selectedOption.getAttribute("_value"));
        // console.log("-----------");
        // console.log(selectedOption);
        // alert(selectedOption.getAttribute("_name"));
        document.body.removeChild(promptDiv);
        cmdChange(this_elem, false);
    };
}


// ----------------------------------------------------------------  events  ---------------------------
document.addEventListener("DOMContentLoaded", function () {
    // Charger les données JSON à partir du fichier de la carte SD
    $.getJSON('/sysConfig.json', function (jsonKeysFile) {
        //console.log(jsonKeysFile.BLE.deviceName);
        // valorise éléments HTML pour chaque valeur de configuration
        document.getElementById("20").value = jsonKeysFile.BLE.deviceName
        document.getElementById("21").value = jsonKeysFile.BLE.deviceManufacturer
        document.getElementById("22").value = jsonKeysFile.WIFI.ssid
        document.getElementById("23").value = jsonKeysFile.WIFI.password
        document.getElementById("24").value = jsonKeysFile.USER.name
        document.getElementById("25").value = jsonKeysFile.USER.surname
        document.getElementById("26").value = jsonKeysFile.POWER.timeOut
        document.getElementById("27").value = jsonKeysFile.USER.defaultCfg
        document.getElementById("28").value = jsonKeysFile.HARDWARE.nbkeys
        document.getElementById("29").value = jsonKeysFile.HARDWARE.id
    });

    // $.getJSON(("/keysConfs/" + jsonKeysFile.USER.defaultCf), function (jsonKeysFile2) {
    //     console.log(jsonKeysFile2.PROGRAM.name);
    //    console.log(jsonKeysFile2);
    // });
});

// -------------------------------------------------------------------
function saveConfig() {
    if (!confirm("Voulez-vous sauvegarder la config ")) {
        return;
    }
    var jsonData = {
        BLE: {
            deviceName: document.getElementById('20').value,
            deviceManufacturer: document.getElementById('21').value
        },
        WIFI: {
            ssid: document.getElementById('22').value,
            password: document.getElementById('23').value
        },
        USER: {
            name: document.getElementById('24').value,
            surname: document.getElementById('25').value,
            defaultCfg: document.getElementById('27').value
        },
        HARDWARE: {
            nbkeys: document.getElementById('28').value,
            id: document.getElementById('29').value
        },
        POWER: {
            timeOut: document.getElementById('26').value
        }
    };
    var xhr = new XMLHttpRequest();
    // Fonction de callback appelée lorsque la requête est terminée
    xhr.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // Print received jsonKeysFile from server
            //result.innerHTML = this.responseText;
            // Traitement de la réponse du serveur (si besoin)
            console.log(this.responseText);
        }
    };
    // Préparation de la requête POST vers l'URL "/submit"
    xhr.open("POST", "/saveConfig", true);
    // Configuration de l'en-tête de la requête
    xhr.setRequestHeader("Content-Type", "application/json;charset=UTF-8");
    // Envoi de la requête POST avec les données JSON dans le corps de la requête
    xhr.send(JSON.stringify(jsonData));
}

// ----------------------------------------------------
function saveKeysConfig(newFile) {

    var fileName = "";
    var pass = false;
    if (!newFile) {  // nom courant
        var selectKeyFile = document.getElementById("listBoxKeysFiles");
        var fileName = selectKeyFile.value;
        if (!confirm("Voulez-vous sauvegarder \nle fichier des touches " + fileName)) {
            return;
        }
    } else {
        do {  // saisie du nouveau nom de fichier
            if (pass) alert("La valeur est incorrecte.[a-z][0-9] (max 20 car)");
            saisie = prompt("Nom du nouveau fichier clavier nom sans le'.json'");
            if (saisie == null) {
                return (false);
            }
            pass = true;
        }
        while (saisie.match(/[^a-zA-Z0-9]/) || saisie == "" || saisie.length > 20); // controle de saisie
        fileName = saisie + ".json";

    }

    console.log("-------- Key fileName ---------- " + fileName);
    console.log("--------jsonKeysFile------------" + jsonKeysFile);

    var jsonTmp = {
        "fileName": "",
        "jsonData": [{}]
    };
    const select = document.getElementById("listBoxKeysFiles");
    jsonTmp.fileName = "/keysConfs/" + fileName;
    //jsonTmp.fileName = "/test.json"; // TODO à enlever pour sauver le bon fichier
    jsonTmp.jsonData = jsonKeysFile;

    //console.log(jsonTmp);
    var json = JSON.stringify(jsonTmp);
    console.log(json);
    var request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        // if (this.readyState == 4 && this.status == 200) {
        //     // Print received jsonKeysFile from server
        //     //result.innerHTML = this.responseText;
        //     // Traitement de la réponse du serveur (si besoin)
        //     console.log(this.responseText);
        // }
        if (request.readyState === XMLHttpRequest.DONE) {
            // Récupération du code de réponse HTTP
            var statusCode = request.status;
            //console.log("Code de réponse : " + statusCode);
            // Traitement de la réponse
            if (statusCode === 200) {
                console.log("La requête a été traitée avec succès !");
                alert("Le fichier a bien enregistré sur la carte SD");
            } else {
                console.log("La requête a échoué avec le code d'état : " + statusCode);
                alert("Erreur.. d'ecriture la carte SD: " + statusCode);
            }
        }
    };
    // Préparation de la requête POST vers l'URL "/submit"
    request.open('POST', "/saveKeysConfig", true);
    // Configuration de l'en-tête de la requête
    request.setRequestHeader('Content-Type', 'application/json');
    // Envoi de la requête POST avec les données JSON dans le corps de la requête
    request.send(json);
    set_listKeysFiles();

}

function deleteKeyFile(fileName) {
    console.log("delete keyFile: " + fileName);
    //var pathName = "/keysConfs/" + fileName;
    var jsonTmp = {
        "fileName": ""
    };
    jsonTmp.fileName = fileName;
    var json = JSON.stringify(jsonTmp);
    // Préparation de la requête POST vers l'URL "/submit"
    var request = new XMLHttpRequest();
    request.onreadystatechange = function () {
        if (this.readyState == 4 && this.status == 200) {
            // Print received jsonKeysFile from server
            //result.innerHTML = this.responseText;
            // Traitement de la réponse du serveur (si besoin)
            console.log(this.responseText);
            alert("Le fichier a bien ete supprimé de la carte SD");
        }
    };
    request.open('POST', "/deleteKeysConfigFile");
    // Configuration de l'en-tête de la requête
    request.setRequestHeader('Content-Type', 'application/json');
    // Envoi de la requête POST avec les données JSON dans le corps de la requête
    request.send(json);
}

function exportKeysConfig() {
    var text = JSON.stringify(jsonKeysFile);
    const select = document.getElementById("listBoxKeysFiles");
    var filename = select.value;

    var blob = new Blob([text], { type: 'text/csv' });
    if (window.navigator.msSaveOrOpenBlob) {
        window.navigator.msSaveBlob(blob, filename);
    } else {
        var elem = window.document.createElement('a');
        elem.href = window.URL.createObjectURL(blob);
        elem.download = filename;
        document.body.appendChild(elem);
        elem.click();
        document.body.removeChild(elem);
    }
    alert("Le fichier " + filename + " a été placé \n dans le repertoire de telechargement");
}

// ------------- UPLOAD --------------   interception du submit pour traitement dans fonction importKeysConfig()
$(document).ready(function () {
    $('#upload-form').submit(function (event) {
        event.preventDefault(); // empêche l'envoi du formulaire par défaut
        importKeysConfig(); // appelle la fonction importKeysConfig()
    });
});

function importKeysConfig() {
    // soumet le formulaire via JavaScript
    var form = document.getElementById("upload-form");
    //var formData = new FormData(form);
    var xhr = new XMLHttpRequest();
    xhr.open("POST", "/upload");
    xhr.onload = function () {
        if (xhr.status === 200) {
            // traitement de la réponse si nécessaire
            var response = JSON.parse(xhr.responseText);
            if (response.success) {
                // afficher une boîte modale de succès
                alert("Le fichier a été importé avec succès !");
            } else {
                // afficher une boîte modale d'erreur
                alert("Une erreur est survenue lors de l'importation du fichier.");
            }
        } else {
            // afficher une boîte modale d'erreur
            alert("Une erreur est survenue lors de la communication avec le serveur.");
        }
        set_listKeysFiles();
    };
    // récupère la modale contenant le formulaire
    var modal = document.getElementById("uploadModal");
    modal.click();
    xhr.send(new FormData(form));
}

// aff aide en markdon
function aff_help() {
    console.log("---- aff document md ---");
    fetch('/help.md')
        .then(response => response.text())
        .then(data => {
            document.getElementById("help").innerHTML = marked.parse(data);
        });
}