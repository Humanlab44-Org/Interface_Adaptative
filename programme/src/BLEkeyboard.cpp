/*
Nom du fichier : BLEkeyboard.ccp
Auteur : Yann Cesbron  (yann@cesbron.org)
Date de création : 4 avril 2023
Licence :  Creative Commons BY-NC-SA
*/

#include "BLEkeyboard.h"

void initBLEKeyboard()
{
    Serial.println(F("-- Initialisation BLEkeyboard"));
    setBLEConfig();
    xTaskSemaphore = xSemaphoreCreateBinary(); // un sémaphore binaire pour le mouseMove (tache partagée clavier/nunchuck)
    xSemaphoreGive(xTaskSemaphore);            // cré le jeton pour 1 premiere utilisation
    Keyboard.begin();
    Mouse.begin();
    Serial.println();
}

// ---------------------------------------  KEYBOARD
void setBLEConfig()
{
    Keyboard.deviceName = getBLEdeviceName();
    Keyboard.deviceManufacturer = getBLEmanufacturer();
    Serial.print(Keyboard.deviceName.c_str());
    Serial.print("/");
    Serial.println(Keyboard.deviceManufacturer.c_str());
    Keyboard.batteryLevel = getVbat();
    changeMacAddress();
}

bool isConnectedBLE()
{
    return (Keyboard.isConnected());
}

void disconnectAll()
{
    Keyboard.disconnectAll();
}

void sendKeyToBLE(String keyValue, int type, int keyState)
{
    if (Keyboard.isConnected())
    {
        Serial.printf("keyValue: %s type: %d keystate: %d\n", keyValue, type, keyState);
        if (keyState == 3)
        {
            Keyboard.releaseAll();
        }
        else
        {
            switch (type)
            {
            case 0:                // chaine de caractere
                if (keyState == 1) // pressed
                {
                    Keyboard.print(keyValue); //  String de 1 à n caracteres
                    // Keyboard.releaseAll();  // evite la repetition
                }
                break;
            case 1: // touches ASCII ou ctrl shift alt...
                Serial.printf("touche ASCII : %x  int %x \n", uint8_t(keyValue[0]), (uint8_t)keyValue.toInt());
                if (keyState == 1) // pressed
                {
                    if (keyValue.toInt() >= 128)
                    {
                        Keyboard.press((uint8_t)keyValue.toInt());
                    }
                    else
                    {
                        Keyboard.press(uint8_t(keyValue[0]));
                    }
                }
                else // released
                {
                    if (keyValue.toInt() >= 128)
                    {
                        Keyboard.release(keyValue.toInt());
                    }
                    else
                    {
                        Keyboard.release(uint8_t(keyValue[0]));
                    }
                }
                break;
            case 2:                // mouse click
                if (keyState == 1) // press
                {
                    // Serial.printf("mouse click press: %x MOUSE_CLICK_LEFT: %x\n", (uint8_t)key.toInt(), MOUSE_LEFT);
                    Serial.printf("mouse click press: %x\n", (uint8_t)keyValue.toInt());
                    Mouse.press((uint8_t)keyValue.toInt());
                }
                else // release
                {
                    Serial.printf("mouse click release : %x \n", (uint8_t)keyValue.toInt());
                    Mouse.release((uint8_t)keyValue.toInt());
                }
                break;
            case 3:                // mouse move
                if (keyState == 1) // press
                {
                    uint8_t dir = (uint8_t)keyValue.toInt();
                    // Serial.println("mouse move: %x ", dir);
                    // start xtask
                    mouseMove(dir, 1);
                }
                else // released
                {
                    mouseMoveKillTask();
                }
                break;
            case 4: // media
                    // TODO   A implementer?
                break;
            default:
                break;
            }
        }
    }
}

void mouseMovevTask(void *pvParameters)
{
    // tache freeRtos
    int mouseSpeedDelay = 30;
    xParams *params = (xParams *)pvParameters; // récupération du pointeur vers les paramètres
    uint8_t nbPts = params->pts;
    uint8_t direction = params->dir;
    // Serial.print("dir: dans la tache: ");
    // Serial.print(direction);
    // Serial.print("  nbPts: dans la tache: ");
    // Serial.println(nbPts);
    for (;;)
    {
        switch (direction)
        {
        case 1: // gauche
                // Serial.println("gauche");
            Mouse.move(-nbPts, 0);
            break;
        case 2: // droite
                // Serial.println("droite");
            Mouse.move(nbPts, 0);
            break;
        case 4: // arriere
                // Serial.println("arriere");
            Mouse.move(0, nbPts);
            break;
        case 8: // avant
                // Serial.println("avant");
            Mouse.move(0, -nbPts);
            break;
        default:
            break;
        }
        mouseSpeedDelay -= 1;
        if (mouseSpeedDelay <= MOUSE_MIN_SPEED_DELAY)
        {
            mouseSpeedDelay = MOUSE_MIN_SPEED_DELAY;
        }
        vTaskDelay(mouseSpeedDelay / portTICK_PERIOD_MS);
    }
}

bool isRunningMouseMovevTask()
{
    eTaskState taskState = eTaskGetState(mouseMovexHandle);

    if (taskState == eRunning || taskState == eReady) // La tâche est en cours d'exécution ou prête à l'être
    {
        // Serial.println("tache demarree");
        return (true);
    }
    else // La tâche n'est pas démarrée
    {
        // Serial.println("tache arretee");
        return (false);
    }
}

void mouseMoveKillTask()
{
    // stop xtask  -> kill task
    if (mouseMovexHandle != NULL)
    {
        // if (isRunningMouseMovevTask())
        // {
        Serial.println(F("- Stop mouse move task"));
        vTaskDelete(mouseMovexHandle);  // kill la tache
        xSemaphoreGive(xTaskSemaphore); // redonne le semaphore
        mouseMovexHandle = NULL;        // NULL -> handle
                                        // }
    }
}

/* Function that creates a task mouseMove. */
void mouseMove(uint8_t direction, uint8_t nbPts)
{
    xParams *params = new xParams;
    Serial.print(F("dir: dans mouseMove: "));
    Serial.print(direction);
    Serial.print(F("  nbPts: dans mouseMove: "));
    Serial.println(nbPts);

    params->dir = direction;
    params->pts = nbPts;
    if (xSemaphoreTake(xTaskSemaphore, 0) == pdTRUE) // On essaie de prendre un jeton, sans bloquer la tâche
    {
        char tskName[] = "MOUSEMOVE";
        BaseType_t xReturned;
        Serial.println(F("* create xtask mouseMove ..."));
        /* Create the task, storing the handle. */

        xReturned = xTaskCreate(
            mouseMovevTask,     /* Function that implements the task. */
            tskName,            /* Text name for the task. */
            2048,               /* Stack size in words, not bytes. */
            (void *)params,     /* Parameter passed into the task. */
            tskIDLE_PRIORITY,   /* Priority at which the task is created. */
            &mouseMovexHandle); /* Used to pass out the created task's handle. */

        delete params;
        if (xReturned == pdPASS)
        {
            /* The task was created.  Use the task's handle to delete the task. */
        }
    }
    else
    {
        Serial.println(F("!tache mouseMove occupee.."));
    }
}
