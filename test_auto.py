import subprocess
import os
import shutil  

# --- CONFIGURATION ---
EXECUTABLE = "./bgrs"
DB_FILE = "inventaire_sauvegarde.txt"
LOG_FILE = "historique.log"

# --- COULEURS DU TERMINAL ---
GREEN = "\033[92m"
RED = "\033[91m"
YELLOW = "\033[93m"
RESET = "\033[0m"

def log(message, type="INFO"):
    if type == "PASS":
        print(f"[{GREEN}PASS{RESET}] {message}")
    elif type == "FAIL":
        print(f"[{RED}FAIL{RESET}] {message}")
    elif type == "WARN":
        print(f"[{YELLOW}WARN{RESET}] {message}")
    else:
        print(f"[INFO] {message}")

def backup_artifacts():
    """Sauvegarde les fichiers de prod actuels en .bak avant les tests"""
    print(f"{YELLOW}--- BACKUP DES DONNÉES ACTUELLES ---{RESET}")
    for f in [DB_FILE, LOG_FILE]:
        if os.path.exists(f):
            backup_name = f + ".bak"
            shutil.copy(f, backup_name)
            log(f"Sauvegarde temporaire : {backup_name}", "INFO")
        else:
            log(f"Pas de fichier {f} à sauvegarder", "INFO")

def restore_artifacts():
    """Restaure les fichiers .bak et écrase les fichiers de test"""
    print(f"\n{YELLOW}--- RESTAURATION DES DONNÉES ---{RESET}")
    for f in [DB_FILE, LOG_FILE]:
        backup_name = f + ".bak"
        if os.path.exists(backup_name):
            shutil.move(backup_name, f) 
            log(f"Restauration : {f}", "PASS")
        else:
            
            if os.path.exists(f):
                os.remove(f)
                log(f"Nettoyage fichier de test (pas d'original) : {f}", "INFO")

def clean_artifacts():
    """Nettoie les fichiers générés pour partir sur une base propre"""
    for f in [DB_FILE, LOG_FILE]:
        if os.path.exists(f):
            os.remove(f)

def compile_project():
    log("Compiling project...", "INFO")
    subprocess.run(["make", "clean"], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    result = subprocess.run(["make"], stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)
    if result.returncode != 0:
        log("Compilation failed!", "FAIL")
        print(result.stderr.decode())
        exit(1)
    log("Compilation successful", "PASS")

def run_scenario(name, inputs, expected_output_snippets=[], valgrind=False):
    input_str = "\n".join(inputs) + "\n"
    
    cmd = [EXECUTABLE]
    if valgrind:
        cmd = ["valgrind", "--leak-check=full", "--error-exitcode=100"] + cmd

    try:
        process = subprocess.Popen(
            cmd,
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True
        )
        stdout, stderr = process.communicate(input=input_str, timeout=3)

        if valgrind and process.returncode == 100:
            log(f"Scenario '{name}': Memory Leak/Error detected via Valgrind!", "FAIL")
            print(stderr)
            return False

        for snippet in expected_output_snippets:
            if snippet not in stdout:
                log(f"Scenario '{name}': Missing expected output '{snippet}'", "FAIL")
                return False

        log(f"Scenario '{name}': Success", "PASS")
        return True

    except subprocess.TimeoutExpired:
        log(f"Scenario '{name}': Timeout (Process hung or infinite loop)", "FAIL")
        process.kill()
        return False
    except Exception as e:
        log(f"Scenario '{name}': Exception {e}", "FAIL")
        return False

# --- DÉBUT DE LA BATTERIE DE TESTS ---

def main():
    print(f"{YELLOW}=== BGRS AUTOMATED TEST SUITE ==={RESET}")
    
    backup_artifacts()

    try:
        compile_project()
        clean_artifacts() 

        
        # Test Basique
        run_scenario("Basic Loot & Display", ["8", "1", "9"], ["Potion de Soin Ultime"], valgrind=True)

        # Test Persistance
        run_scenario("Persistence (Save)", ["8", "6", "9"], ["Sauvegarde terminee"], valgrind=False)
        if os.path.exists(DB_FILE):
            log("Save file created physically.", "PASS")
        else:
            log("Save file missing!", "FAIL")

        run_scenario("Persistence (Load)", ["7", "1", "9"], ["Chargement termine", "Duct tape"], valgrind=True)

        # Test Robustesse entrées
        run_scenario("Input Sanitization", ["2", "BadItem", "Desc", "Cat", "-10", "badtext", "5", "-5.5", "10.0", "0", "None", "1", "9"], ["Quantite :", "BadItem"], valgrind=True)

        # Test Buffer Overflow
        run_scenario("Buffer Limits", ["2", "LongItem", "A"*1020, "Cat", "1", "1", "0", "Note", "1", "9"], ["LongItem"], valgrind=True)

        # Test Corruption Fichier (Ecriture forcée)
        with open(DB_FILE, "w") as f:
            f.write("1|ItemCorrompu|Desc|Cat|10|5.5|0|Note\n") 
            f.write("2|ItemCasseDesc|Cat|10|5.5|0|Note\n")    
        
        run_scenario("File Corruption Resilience", ["7", "1", "9"], ["Warning", "ItemCorrompu"], valgrind=True)

        # Tests de logique
        run_scenario("Empty List Ops", ["1", "3", "1", "9"], ["Inventaire vide"], valgrind=True)
        
        run_scenario("Linked List Deletion", ["8", "3", "1", "3", "4", "3", "7", "1", "9"], ["[-] Produit supprime"], valgrind=True)
        
        # Test Modification et persistance des valeurs si Entrée vide
        run_scenario("Modif Persistence", ["8", "4", "1", "\n", "\n", "\n", "\n", "\n", "\n", "\n", "1", "9"], ["[~] Modification reussie"], valgrind=True)
        
        run_scenario("Modif Bad ID", ["8", "4", "999", "9"], ["Produit introuvable"], valgrind=True)

    except KeyboardInterrupt:
        print(f"\n{RED}[!] Tests interrompus par l'utilisateur.{RESET}")
    except Exception as e:
        print(f"\n{RED}[!] Erreur inattendue : {e}{RESET}")
    finally: # restauration même en cas de crash

        restore_artifacts()
        print(f"\n{YELLOW}=== TESTS COMPLETED (Environment Restored) ==={RESET}")

if __name__ == "__main__":
    main()