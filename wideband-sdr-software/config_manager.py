import json
import os
import shutil
from typing import Any, Dict

class ConfigManager:
    def __init__(self, config_path: str = "config/default_config.json", schema_path: str = "config/sdr_config_schema.json"):
        self.config_path = config_path
        self.schema_path = schema_path
        self.config = self.load_config()

    def load_config(self) -> Dict[str, Any]:
        if not os.path.exists(self.config_path):
            return {}
        with open(self.config_path, 'r') as f:
            return json.load(f)

    def save_config(self, config: Dict[str, Any], path: str = None):
        target_path = path or self.config_path
        with open(target_path, 'w') as f:
            json.dump(config, f, indent=4)
        self.config = config

    def validate_config(self, config: Dict[str, Any]) -> bool:
        """Perform basic validation of configuration structure and ranges."""
        try:
            # Check required sections
            for section in ["system", "processing", "network"]:
                if section not in config:
                    print(f"Missing section: {section}")
                    return False

            # System validation
            sys = config["system"]
            if not (1_000_000 <= sys.get("frequency", 0) <= 10_000_000_000):
                return False
            if not (1_000 <= sys.get("sample_rate", 0) <= 105_000_000):
                return False

            # Processing validation
            proc = config["processing"]
            if proc.get("mode") not in [0, 1, 2]:
                return False

            # Network validation
            net = config["network"]
            if "ip_address" not in net or "port" not in net:
                return False

            return True
        except Exception as e:
            print(f"Validation error: {e}")
            return False

    def backup_config(self, backup_path: str):
        shutil.copy(self.config_path, backup_path)

    def restore_config(self, backup_path: str):
        if os.path.exists(backup_path):
            shutil.copy(backup_path, self.config_path)
            self.config = self.load_config()

if __name__ == "__main__":
    cm = ConfigManager()
    print(f"Loaded config: {cm.config}")
    if cm.validate_config(cm.config):
        print("Configuration is valid.")
    else:
        print("Configuration is invalid!")
