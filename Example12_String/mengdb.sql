/*
Navicat MySQL Data Transfer

Source Server         : 盟创
Source Server Version : 50712
Source Host           : 123.56.46.195:3306
Source Database       : mengdb

Target Server Type    : MYSQL
Target Server Version : 50712
File Encoding         : 65001

Date: 2016-07-22 11:21:03
*/

SET FOREIGN_KEY_CHECKS=0;

-- ----------------------------
-- Table structure for `migrations`
-- ----------------------------
DROP TABLE IF EXISTS `migrations`;

-- ----------------------------
-- Table structure for `myun_client_list`
-- ----------------------------
DROP TABLE IF EXISTS `myun_client_list`;
CREATE TABLE `myun_client_list` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `online` tinyint(1) NOT NULL,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `type` smallint(6) unsigned DEFAULT NULL,
  `ip` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `update_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`,`device_id`),
  UNIQUE KEY `myun_client_list_device_id_unique` (`device_id`)
) ENGINE=MyISAM AUTO_INCREMENT=63 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_client_list
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_client_log`
-- ----------------------------
DROP TABLE IF EXISTS `myun_client_log`;
CREATE TABLE `myun_client_log` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  `type` int(11) unsigned DEFAULT '0',
  `log` varchar(255) DEFAULT NULL,
  `ip` varchar(255) DEFAULT NULL,
  `time` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB AUTO_INCREMENT=1678 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of myun_client_log
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_debug_data`
-- ----------------------------
DROP TABLE IF EXISTS `myun_debug_data`;
CREATE TABLE `myun_debug_data` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `type` smallint(5) unsigned NOT NULL,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `data` text COLLATE utf8_unicode_ci NOT NULL,
  `create_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=329 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_debug_data
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_device_store`
-- ----------------------------
DROP TABLE IF EXISTS `myun_device_store`;
CREATE TABLE `myun_device_store` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `info` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_device_store
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_mobile_data`
-- ----------------------------
DROP TABLE IF EXISTS `myun_mobile_data`;
CREATE TABLE `myun_mobile_data` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `sensor_data` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `create_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=70468 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_mobile_data
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_mobile_state`
-- ----------------------------
DROP TABLE IF EXISTS `myun_mobile_state`;
CREATE TABLE `myun_mobile_state` (
  `device_id` char(16) COLLATE utf8_unicode_ci NOT NULL,
  `state_working` int(11) unsigned NOT NULL,
  `create_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`device_id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_mobile_state
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_mobile_statis`
-- ----------------------------
DROP TABLE IF EXISTS `myun_mobile_statis`;
CREATE TABLE `myun_mobile_statis` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_index` int(10) unsigned NOT NULL,
  `val0` int(11) NOT NULL,
  `val1` int(11) NOT NULL,
  `val2` int(11) NOT NULL,
  `val3` int(11) NOT NULL,
  `val4` int(11) NOT NULL,
  `val5` int(11) NOT NULL,
  `val6` int(11) NOT NULL,
  `val7` int(11) NOT NULL,
  `val8` int(11) NOT NULL,
  `val9` int(11) NOT NULL,
  `val10` int(11) NOT NULL,
  `val11` int(11) NOT NULL,
  `val12` int(11) NOT NULL,
  `val13` int(11) NOT NULL,
  `val14` int(11) NOT NULL,
  `val15` int(11) NOT NULL,
  `time` int(10) unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_mobile_statis
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_nc_data`
-- ----------------------------
DROP TABLE IF EXISTS `myun_nc_data`;
CREATE TABLE `myun_nc_data` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `time` timestamp,
  `index` tinyint(4) NOT NULL,
  `inhibition` double(8,2) NOT NULL,
  `temp` double(8,2) NOT NULL,
  `humidity` double(8,2) NOT NULL,
  `contrast_a` double(8,2) NOT NULL,
  `pass` tinyint(4) NOT NULL,
  `location` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `master` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `surveyor` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `lat` double NOT NULL DEFAULT '0',
  `lon` double NOT NULL DEFAULT '0',
  `under_test` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `plant` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `standard` double(8,2) NOT NULL,
  `hash` char(40) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `myun_nc_data_hash_unique` (`hash`)
) ENGINE=MyISAM AUTO_INCREMENT=63 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_nc_data
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_pocket_data`
-- ----------------------------
DROP TABLE IF EXISTS `myun_pocket_data`;
CREATE TABLE `myun_pocket_data` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `data` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `lat` double NOT NULL DEFAULT '0',
  `lon` double NOT NULL DEFAULT '0',
  `hash` char(40) COLLATE utf8_unicode_ci NOT NULL,
  `json` json DEFAULT NULL,
  `create_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  UNIQUE KEY `myun_pocket_data_hash_unique` (`hash`)
) ENGINE=MyISAM AUTO_INCREMENT=1941 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_pocket_data
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_pocket_device`
-- ----------------------------
DROP TABLE IF EXISTS `myun_pocket_device`;
CREATE TABLE `myun_pocket_device` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `device_id` bigint(20) DEFAULT NULL,
  `air_temp` float DEFAULT NULL COMMENT '空气温度',
  `air_humi` float DEFAULT NULL COMMENT '空气湿度',
  `air_co2` float DEFAULT NULL COMMENT '二氧化碳含量',
  `soil_temp` float DEFAULT NULL COMMENT '土壤温度',
  `soil_humi` float DEFAULT NULL COMMENT '土壤湿度',
  `soil_pene` float DEFAULT NULL COMMENT '土壤渗透率',
  `ph_vol` float DEFAULT NULL COMMENT 'PH值',
  `salt_vol` float DEFAULT NULL COMMENT '盐分',
  `sun_vol` float DEFAULT NULL COMMENT '照度',
  `ec_vol` float DEFAULT NULL COMMENT '电导率',
  `time` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=117 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of myun_pocket_device
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_soil_data`
-- ----------------------------
DROP TABLE IF EXISTS `myun_soil_data`;
CREATE TABLE `myun_soil_data` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `type` smallint(5) unsigned NOT NULL,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `data` text COLLATE utf8_unicode_ci NOT NULL,
  `hash` char(40) COLLATE utf8_unicode_ci NOT NULL,
  `create_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP,
  `json` json DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `myun_soil_data_hash_unique` (`hash`)
) ENGINE=MyISAM AUTO_INCREMENT=381 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of myun_soil_data
-- ----------------------------

-- ----------------------------
-- Table structure for `myun_std_log`
-- ----------------------------
DROP TABLE IF EXISTS `myun_std_log`;
CREATE TABLE `myun_std_log` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `app` varchar(255) DEFAULT NULL COMMENT 'record app type',
  `log` text,
  `trace` varchar(255) DEFAULT NULL,
  `color` int(11) DEFAULT NULL,
  `time` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=19709 DEFAULT CHARSET=utf8;

-- ----------------------------
-- Records of myun_std_log
-- ----------------------------

-- ----------------------------
-- Table structure for `password_resets`
-- ----------------------------
DROP TABLE IF EXISTS `password_resets`;
CREATE TABLE `password_resets` (
  `email` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `token` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `created_at` timestamp NOT NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
  KEY `password_resets_email_index` (`email`),
  KEY `password_resets_token_index` (`token`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of password_resets
-- ----------------------------

-- ----------------------------
-- Table structure for `user_devices`
-- ----------------------------
DROP TABLE IF EXISTS `user_devices`;
CREATE TABLE `user_devices` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `account` int(10) unsigned NOT NULL,
  `device_id` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `proto_device_id` varchar(255) COLLATE utf8_unicode_ci DEFAULT NULL,
  `type` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `created_at` timestamp,
  `updated_at` timestamp,
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `json` json DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=MyISAM AUTO_INCREMENT=39 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of user_devices
-- ----------------------------

-- ----------------------------
-- Table structure for `users`
-- ----------------------------
DROP TABLE IF EXISTS `users`;
CREATE TABLE `users` (
  `id` int(10) unsigned NOT NULL AUTO_INCREMENT,
  `name` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `email` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `password` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `remember_token` varchar(100) COLLATE utf8_unicode_ci DEFAULT NULL,
  `leader` int(10) unsigned NOT NULL,
  `privilege` enum('普通','受限','游客','开发','管理员') COLLATE utf8_unicode_ci NOT NULL DEFAULT '普通',
  `phone` varchar(255) COLLATE utf8_unicode_ci NOT NULL,
  `balance` decimal(10,2) NOT NULL,
  `created_at` timestamp,
  `updated_at` timestamp,
  PRIMARY KEY (`id`),
  UNIQUE KEY `users_email_unique` (`email`)
) ENGINE=MyISAM AUTO_INCREMENT=12 DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- ----------------------------
-- Records of users
-- ----------------------------

