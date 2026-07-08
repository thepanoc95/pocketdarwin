// Template implementation for Device Tree extension
// This file provides additional DTS capabilities for device bundles

#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

/**
 * @brief Device Tree representation for device bundles
 */
class DeviceTree {
public:
    struct Node {
        std::string name;
        std::map<std::string, std::string> properties;
        std::vector<std::shared_ptr<Node>> children;
        
        void addProperty(const std::string& key, const std::string& value) {
            properties[key] = value;
        }
        
        void addChild(const std::string& nodeName) {
            auto child = std::make_shared<Node>();
            child->name = nodeName;
            children.push_back(child);
        }
    };
    
public:
    DeviceTree() : root_(std::make_shared<Node>()) {
        root_->name = "/";
    }
    
    bool parseDTS(const std::string& dtsContent);
    bool compileToDTB(const std::string& outputPath) const;
    std::string compileToDTBString() const;
    
    std::shared_ptr<Node> getRoot() const { return root_; }
    std::shared_ptr<Node> findNode(const std::string& path) const;
    
    std::string toString() const;
    
private:
    std::shared_ptr<Node> root_;
    
    std::shared_ptr<Node> _parseNode(const std::string& nodeSource);
    std::vector<std::string> _tokenize(const std::string& str) const;
};
