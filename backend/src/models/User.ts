import { Policy } from "./Organisation";
import { BaseEntity } from "./BaseEntity";

export default interface User extends BaseEntity {
    firstName: string,
    lastName: string,
    email: string,
    password: string, // hash 
    role: Policy
}